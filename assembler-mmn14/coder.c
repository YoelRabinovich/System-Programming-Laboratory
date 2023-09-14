#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "files.h"
#include "coder.h"

/*********************************** Variables ***********************************/

/* The instructions counter holding the address where the next instruction/operand word will go */
static unsigned int IC = START_ADDRESS;

/* The machine code (instructions/operands) to be generated will accumulate here */
static union Cod* img_code;

/* To keep track of the types of each word entered into the code output array (i.e. instruction or operand) */
static WordType* types_word;

/* The data words (string/int) to be generated will accumulate here */
static int* img_data;

/* The data counter pointing to where in the data image the next data word will go */
static unsigned int DC = 0;

/* Needed to write extern file */
extern int sym_ref_i;
extern  InfoSym* ref_symbol;


/*********************************** Functions ***********************************/

/* set code image array - returns 1 if success, 0 if failure */
int set_img_code(size_t n) {
    IC = START_ADDRESS;
    img_code = (Cod*)malloc(sizeof(Cod) * n);
    return img_code != NULL;
}

/* set data image array - returns 1 if success, 0 if failure */
int set_img_data(size_t n) {
    DC = 0;
    img_data = (int*)malloc(sizeof(int) * n);
    return img_data != NULL;
}

/* set word_types array - returns 1 if success, 0 if failure */
int set_type(size_t n) {
    types_word = (WordType*)malloc(sizeof(WordType) * n);
    return types_word != NULL;
}

/* Frees memory allocated for code image, data image and word_types array */
void memory_free() {
    free(img_code);
    free(img_data);
    free(types_word);
}

/* Symbol table needs to know the current IC when adding new symbol */
unsigned int get_IC() {
    return IC;
}

/* Symbol table needs to know the current DC when adding new symbol */
unsigned int get_DC() {
    return DC;
}

/* Adds instruction word to the code image */
void inst_adder(int opcode, addressing_type firstAdd, int firstReg, addressing_type secondAdd, int secondReg, int funct) {
    union Cod word;
    Instruction instruction;
    int index;
    index = IC - START_ADDRESS;
    instruction.opcode = opcode;
    instruction.first_arg_type = firstAdd;
    instruction.first_reg = firstReg;
    instruction.second_arg_type = secondAdd;
    instruction.second_reg = secondReg;
    instruction.funct = funct;
    instruction.info_linker = A;
    word.instruction = instruction;
    img_code[index] = word;
    types_word[index] = INSTRUCTION;
    IC++;
}

/* Edits operand word in the code image whose address and linker info was missing */
void op_edit(int ic, char*label, addressing_type type) {
    int index;
    Operand* operand;
    Symbol* symbol;

    index = ic - START_ADDRESS;
    symbol = symbol_search(label);
    if (symbol == NULL) {
        return;
    }

    if (types_word[index] == OPERAND) {
        operand = &(img_code[index].operand);
        if (type == DIRECT) {
            operand->value = symbol->address;
            if (symbol->loc == LOC_EXTERNAL) {
                operand->info_linker = E;
            }
            else {
                operand->info_linker = R;
            }
        }
        else if (type == RELATIVE) {
            operand->value = symbol->address - ic + 1;
            operand->info_linker = A;
        }
    }
}

/* Add an operand word to the code image */
void op_adder(int value, InfoLink linker_info) {
    union Cod word;
    Operand operand;
    int index;
    index = IC - START_ADDRESS;
    operand.value = value;
    operand.info_linker = linker_info;
    word.operand = operand;
    img_code[index] = word;
    types_word[index] = OPERAND;
    IC++;
}

/* Add a data word to the data image */
void data_to_stack(int data) {
    img_data[DC++] = data;
}

/* converts a number to 2's complement */
int to_comp_two(int val) {
    if (val < 0) {
        return (1 << 24) + val;
    }
    return val;
}

/* encodes an instruction word */
int add_instuc(Instruction instruction) {
    return
        (instruction.opcode << 18) + \
        (instruction.first_arg_type << 16) + \
        (instruction.first_reg << 13) + \
        (instruction.second_arg_type << 11) + \
        (instruction.second_reg << 8) + \
        (instruction.funct << 3) + \
        (instruction.info_linker);
}

/* encodes an operand word (2s complement) */
int add_ope(Operand operand) {
    return to_comp_two(operand.value << 3) + (operand.info_linker);
}

/* Generate the machine code to .ob file */
void ob_file(char* file_path) {
    int i;
    int address;
    WordType wordType;
    FILE *fp;
    fp = fopen(file_path, "w");

    address = START_ADDRESS;
    if (fp) {
        /* Header */
        fprintf(fp, "%7i %-6i\n", IC - START_ADDRESS, DC);

        /* Code section: */
        for (i = 0; i < IC - START_ADDRESS; i++, address++) {
            address_to_file(fp, address);
            wordType = types_word[i];
            switch (wordType) {
                case INSTRUCTION:
                    num_to_file(fp, add_instuc(img_code[i].instruction));
                    break;
                case OPERAND:
                    num_to_file(fp, add_ope(img_code[i].operand));
                    break;
                case DATA: {/* not relevant here */}
            }
        }

        /* Data section: */
        for (i = 0; i < DC; i++, address++) {
            address_to_file(fp, address);
            num_to_file(fp, to_comp_two(img_data[i]));
        }
        fclose(fp);
    } else {
        errors_num++;
    }
}

/* Generate the .ext file */
void ext_file(char* file_path) {
    int i;
    int address;
    char* label = NULL;
    Symbol* symbol;
    FILE *fp;
    fp = fopen(file_path, "w");
    if (fp) {
        for (i = 0; i < sym_ref_i; i++) {
            address = ref_symbol[i].IC;
            label = ref_symbol[i].label;
            symbol = symbol_search(label);
            if (symbol != NULL && symbol->loc == LOC_EXTERNAL) {
                fprintf(fp, "%s ", label);
                address_to_file(fp, address);
                fprintf(fp, "\n");
            }

        }
        fclose(fp);
    }
    else {
        errors_num++;
    }
}
