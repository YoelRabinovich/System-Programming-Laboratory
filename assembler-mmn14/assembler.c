#include <stdlib.h>

#include "line_parser.h"
#include "assembler.h"
#include "coder.h"
#include "passes.h"
#include "files.h"


/*********************************** Global variables ***********************************/

/* Error tracking */
int errors_num = 0;

char line[LINE_LEN];

/* Keep track of source file line_num (including blank lines) to indicate the line number in case of errors  */
int line_number = 0;

/* Keep track of valid parsed lines (this is the length of the parsed_lines array below) */
int num_lines = 0;

/*  Processed input lines will be stored in an array of structured data */
ParsedLine** parsed_lines;

/* tracks the number of symbols during the pre-processing stage */
int symbol_num = 0;

/* tracks the number of words (instruction/operand) that will be generated so we can efficiently allocate the correct size array for the code/data images (used in the pre-processing stage)*/
int num_words_code = 0;

/* tracks the number of data words that will be generated so we can efficiently allocate the correct size array for the code/data images (used in the pre-processing stage)*/
int num_words_data = 0;

/* InfoSym for instances of symbol references are stored in pass 1 for use in pass 2 */
InfoSym* ref_symbol;
int sym_ref_i = 0;
int n_symbol_refs = 0;

/* Initializing the 8 predefined registers */
Reg regi_table[] = {
    {"r0", 0},
    {"r1", 1},
    {"r2", 2},
    {"r3", 3},
    {"r4", 4},
    {"r5", 5},
    {"r6", 6},
    {"r7", 7}
};

/* Construct specification info for the 4 assembler directives ('.entry', '.extern', '.data', '.string') */
Directive directives[4] = {
    {".string", 1, STRING},  
    {".data", 999999, INT}, 
    {".entry", 1, LABEL}, 
    {".extern", 1, LABEL} 
};

/* table for the 16 instruction operations: Each op takes 0-2 args (operands), each can be used only with certain addressing modes
 * Format: {<op name>, <opcode>, <funct>, <num of args>, {src arg addr modes>, <dest arg addr modes>}
 */
Op opcode_table[] = {
    {"mov", 0, 0, 2, {1, 1, 0, 1}, {0, 1, 0, 1}},
    {"cmp", 1, 0, 2, {1, 1, 0, 1}, {1, 1, 0, 1}},
    {"add", 2, 1, 2, {1, 1, 0, 1}, {0, 1, 0, 1}},
    {"sub", 2, 2, 2, {1, 1, 0, 1}, {0, 1, 0, 1}},
    {"lea", 4, 0, 2, {0, 1, 0, 0}, {0, 1, 0, 1}},
    {"clr", 5, 1, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"not", 5, 2, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"inc", 5, 3, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"dec", 5, 4, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"jmp", 9, 1, 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
    {"bne", 9, 2, 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
    {"jsr", 9, 3, 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
    {"red", 12, 0, 1, {0, 0, 0, 0}, {0, 1, 0, 1}},
    {"prn", 13, 0, 1, {0, 0, 0, 0}, {1, 1, 0, 1}},
    {"rts", 14, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {"stop", 15, 0, 0, {0, 0, 0, 0}, {0, 0, 0, 0}}
};


/*********************************** Main ***********************************/ 

/*
Explanation of the stages of the program:
- Syntax analysis and appropriate allocation of memory (pre-processing stage).  important: if there are errors do not continue
- Crossing the legal input with the help of the 2 transitions and creating suitable structures
- End the program and create an appropriate output
*/
int main(int argc, char * argv[]) {
    FILE* fp;
    int inputs_i;
    char * path_input;
    int rc = 0;
    ParsedLine *parsed_line;

    if (argc < 2) {
        printf("No input files specified.\nUsage: assembler <file1> [<file2> <file3> ...]\n");
        return 1;
    }

    /* Process each .as file given in the cmd line input */
    for (inputs_i = 1; inputs_i < argc; inputs_i++) {
        count_reset();

        path_input = file_name(argv[inputs_i], ".as");
        fp = fopen(path_input, "r");
        if (fp == NULL) {
            fprintf(stderr, "Error: Unable to open '%s'\n", path_input);
            continue;
        }

        printf("\nWorking on file: \'%s\'\n", path_input);

        /* Pre-processing stage: Parse, validate and restructure input file line by line: */
        while (fgets(line, sizeof(line), fp) != NULL) {
            line_number++;
            parsed_line = parsed_again(line_number, line);
            if (parsed_line != NULL && add_parsed_line(parsed_line)) {
                /* Keep track of how many entries we will have to allocate for the symbol table: */
                symbol_num += num_of_sym(parsed_line);

                /* Keep track of how many words we will have to allocate for the code image: */
                num_words_code += num_of_code(parsed_line);

                /* Keep track of how many words we will have to allocate for the data image: */
                num_words_data += num_of_data(parsed_line);

                /* Keep track of how many symbol references we need to allocate for: */
                n_symbol_refs += how_many_refs(parsed_line);
            }
        }
        fclose(fp);
        free(path_input);

        if (errors_num) { /* no point in carrying on to next stage */
            printf("*** Syntax checker found %i errors. Skipping file. ***\n", errors_num);
            free_parsed_lines();
            rc |= errors_num;
            continue;
        }

        /* Allocate memory for the assembler stages: */
        if (!set_img_code(num_words_code) ||
            !set_img_data(num_words_data) ||
            !set_type(num_words_code + num_words_data) ||
            !init_symbol_refs()) {

            printf("*** Memory allocation error. Skipping file. ***.\n");
            errors_num++;
            rc |= errors_num;
            continue;
        }

        /* Do the 'first pass' on the validated and structured input to build symbol table and
         * to start encoding machine code output */
        do_pass_one();
        if (errors_num) { /* no point in carrying on to next stage */
            printf("*** %i errors found in first pass. Skipping file. ***\n", errors_num);
            free_memory();
            rc |= errors_num;
            continue;
        }

        /* Do the 'second pass' to fill missing info from the completed symbol table */
        do_pass_two();

        /* If no errors, generate output files */
        if (!errors_num) {
            make_output_files(argv[inputs_i]);
        }
        else {
            printf("*** %i errors found in second pass. Skipping file. ***\n", errors_num);
            rc |= errors_num;
        }
        free_memory();
    }
    return rc > 0;
}

/*********************************** Struct functions ***********************************/

/* Finds register info by name Returns -1 if invalid */
int find_reg(char* reg_name) {
    int i;
    for (i = 0; i < NUMBER_OF_REG; i++) {
        if (strcmp(reg_name, regi_table[i].name) == 0) {
            return regi_table[i].id;
        }
    }
    return -1;
}

/* Fetches directive info by name - Returns NULL if invalid */
Directive* get_directive(char* directive_name) {
    int i;
    for (i = 0; i < NUMBER_OF_DIREC; i++) {
        if (strcmp(directives[i].name, directive_name) == 0) {
            return &directives[i];
        }
    }
    return NULL;
}

/* Finds op info by name - Returns NULL if invalid */
Op* get_op(char* op) {
    int i;
    for (i = 0; i < NUMBER_OF_OPS; i++) {
        if (strcmp(opcode_table[i].name, op) == 0) {
            return &opcode_table[i];
        }
    }
    return NULL;
}


/*********************************** Help functions ***********************************/

/* Add a new parsed line (allocating memory if needed) */
int add_parsed_line(ParsedLine* parsed_line) {
    if (num_lines == 0) { /* need to allocate initial memory */
        parsed_lines = (ParsedLine**)malloc(sizeof(ParsedLine*) * INPUT_SIZE);
        if (parsed_lines == NULL) {
            errors_num++;
            printf("Failed to allocate memory for parsing input lines\n");
            return 0;
        }
    }
    else if (num_lines % INPUT_SIZE == 0) { /* need to resize the array */
        ParsedLine** tmp = (ParsedLine**)realloc(parsed_lines, sizeof(ParsedLine*) * (num_lines / INPUT_SIZE) + 1);
        if (tmp == NULL) {
            errors_num++;
            printf("Failed to reallocate memory for parsing input lines\n");
            return 0;
        }
        parsed_lines = tmp;
    }
    parsed_lines[num_lines++] = parsed_line;
    return 1;
}

/* Free parsed_lines memory after each input file is done */
void free_parsed_lines() {
    int i_line;
    if (num_lines > 0) {
        for (i_line = 0; i_line < num_lines; i_line++) {
            parsed_free(parsed_lines[i_line]);
        }
        num_lines = 0;
        free(parsed_lines);
    }
}

/* init symbol_references array */
int init_symbol_refs() { /* free memory after each input file is done */
    sym_ref_i = 0;
    ref_symbol = (InfoSym*)malloc(sizeof(InfoSym) * n_symbol_refs);
    return ref_symbol != NULL;
}

/* init symbol_references array */
void free_symbol_refs() { /* free memory after each input file is done */
    int i;
    for (i = 0; i < n_symbol_refs; i++) {
        free(ref_symbol[i].label);
    }
    free(ref_symbol);
    sym_ref_i = 0;
    n_symbol_refs = 0;
}

/* Free all memory after each input file is done */
void free_memory() {
    free_parsed_lines();
    memory_free();
    sym_t_free();
    free_symbol_refs();
}

/* to_string function for LinkerInfo enum */
char* str_info(InfoLink linker_info) {
    switch (linker_info) {
        case Linker_UNK: return "UNK";
        case E:   return "E";
        case R: return "R";
        case A: return "A";
        default: return "Unknown LinkerInfo";
    }
}

/* to_string function for DirectiveArgType enum */
char* to_str(ArgType arg_type) {
    switch (arg_type) {
        case LABEL: return "LABEL";
        case INT:   return "INT";
        case STRING: return "STRING";
        default: return "Unknown DirectiveArgType";
    }
}

/* to_string function for AddrMode enum */
char* addr_mode_str(addressing_type mode) {
    switch (mode) {
        case IMMEDIATE: return "IMMEDIATE";
        case DIRECT:   return "DIRECT";
        case RELATIVE: return "RELATIVE";
        case REGISTER: return "REGISTER";
        default: return "Unknown Addrmode";
    }
}

/* Output files are created (if 0 errors)  */
void make_output_files(char *output_path) {
    char* path;
    path = file_name(output_path, ".ob");
    ob_file(path);
    printf("  -> Successfully created %s\n", path);
    free(path);

    path = file_name(output_path, ".ext");
    ext_file(path);
    printf("  -> Successfully created %s\n", path);
    free(path);

    path = file_name(output_path, ".ent");
    write_entry(path);
    printf("  -> Successfully created %s\n", path);
    free(path);
}

/* counters reset before processing each file */
void count_reset() {
    errors_num = 0;
    line_number = 0;
    num_lines = 0;
    symbol_num = 0;
    num_words_code = 0;
    num_words_data = 0;
    sym_ref_i = 0;
}