#include <string.h>

#include "passes.h"
#include "string.h"
#include "coder.h"
#include "table.h"
#include "line_parser.h"

/*  In the first transition the assembler builds the table of symbols and the skeleton of the memory image (instructions and data).
The assembler reads the source file line by line, and acts according to the type of line (instruction, guidance, or blank line / note). */
 void do_pass_one() {
     int line_i;
     errors_num = 0;
     line_number = 0;

     for (line_i = 0; line_i < num_lines; line_i++) {
         ParsedLine* parsed_line = parsed_lines[line_i];
         line_number = parsed_line->number_line;
         if (parsed_line->op != NULL) { /* a code instruction word */
             operand_handler(parsed_line);
         }
         else if (parsed_line->directive != NULL) { /* an assembler directive */
             directive_handle(parsed_line);
         }
     }

     /* Also update data addresses in symbol table by moving them by the number of words in the code section - data start after the code section in memory: */
     data_move();
}

/* In the second transition, the assembler completes using the symbol table the encoding of all the words in the set of instructions that have not yet been coded in the first transition
In our machine model these are additional information words of commands, which encode operand by a direct or relative addressing method.
The operand contains a symbol that is defined as internal or external, so in the field E, R, A the bit R or bit E, respectively, will be 1 */
void do_pass_two() {
    int i;
    int line_i;
    line_number = 0;
    errors_num = 0;

    /* Update symbols from 'entry' directives with the 'entry' attribute in the table */
    for (line_i = 0; line_i < num_lines; line_i++) {
        ParsedLine* parsed_line = parsed_lines[line_i];
        line_number = parsed_line->number_line;
        if (parsed_line->directive != NULL && strcmp(parsed_line->directive, ".entry") == 0) {
            entry_sym_up(parsed_line->args[0]);
        }
    }

    /* filling in addresses and linker info (A-R-E) for label operands that were referenced - using direct and relative address modes.  */
    for (i = 0; i < sym_ref_i; i++) {
        InfoSym symbol_info = ref_symbol[i];
        line_number = symbol_info.number_line;
        op_edit(symbol_info.IC, symbol_info.label, symbol_info.adder_type);
    }
}

/* Generates instruction words and operand words for lines with an 'op' in the source code to go into the code image (Used in the first pass). 
Some info will be missing due to label references whose addresses have not yet been entered into the symbol table - will be filled in in the second pass . */
void operand_handler(ParsedLine *parsed_line) {
    int arg_i;
    int first_reg = 0;
    int second_reg = 0;
    char* first_arg = NULL;
    char* second_arg = NULL;
    Op* op;
    addressing_type first_add_type = IMMEDIATE;
    addressing_type second_add_type = IMMEDIATE;

    /* Enter label (if there is one) into symbol table before adding new code */
    if (parsed_line->label != NULL) {
        sym_add(parsed_line->label, TYPE_CODE, LOC_UNK);
    }

    /* Add instruction word to code image: */
    op = get_op(parsed_line->op);
    if (op == NULL) { /* can't happen since we are already passed pre-processing stage but in any case ... */
        return;
    }
    /* Determine addr mode and register fields for each operand */
    for (arg_i = 0; arg_i < parsed_line->num_of_args; arg_i++) {
        char* arg = parsed_line->args[arg_i];
        addressing_type mode = add_type(arg);
        if (arg_i == 0 && parsed_line->num_of_args == 2) { /* so this is the source arg */
            first_arg = copy_str(arg);
            first_add_type = mode;
            if (mode == REGISTER) {
                first_reg = find_reg(arg);
            }
        }
        else { /* dest arg */
            second_arg = copy_str(arg);
            second_add_type = mode;
            if (mode == REGISTER) {
                second_reg = find_reg(arg);
            }
        }
    }
    inst_adder(op->opcode, first_add_type, first_reg, second_add_type, second_reg, op->funct);

    /* Add an operand word for each (non-register) arg: */
    if (first_arg != NULL) {
        handle_operand(first_arg, first_add_type);
    }
    if (second_arg != NULL) {
        handle_operand(second_arg, second_add_type);
    }
}

/* Adds operand for (non-register) arg :  For IMMEDIATE adding the full information (Used in first pass) . For DIRECT and RELATIVE operands - placeholder,
 stored some other information on the side to be used in the second pass */
void handle_operand(char *arg, addressing_type addr_mod) {
    switch (addr_mod) {
        case IMMEDIATE: /* first remove the '#' prefix */
            op_adder(int_val(arg, 1), A);
            return;
        case RELATIVE: /* first remove the '&' prefix */
            arg = sub_get(arg, 1, strlen(arg));
        case DIRECT: {
            InfoSym symbolInfo;
            symbolInfo.number_line = line_number;
            symbolInfo.IC = get_IC();
            symbolInfo.label = arg;
            symbolInfo.adder_type = addr_mod;
            ref_symbol[sym_ref_i++] = symbolInfo;
            op_adder(0, Linker_UNK);
        case REGISTER:
            {/* these are encoded inside the instruction word and do not generate operand words */}
        }
    }
}

/* Detect AddrMode of input arg */
addressing_type add_type(char* str) {
    if (str[0] == '#') {
        return IMMEDIATE;
    }
    if (str[0] == '&') {
        return RELATIVE;
    }
    if (find_reg(str) != -1) {
        return REGISTER;
    }
    return DIRECT;
}

/* enter numerical and string data into data image and symbol table  and also extern symbols into the symbol table (Used in the first pass). */
void directive_handle(ParsedLine *parsed_line) {
    int i_arg;
    int i;

    if (strcmp(parsed_line->directive, ".data") == 0 || strcmp(parsed_line->directive, ".string") == 0) {
        /* Enter data symbol (if there is was a label in the src code) into symbol table,
         * and then add the new integer/string data: */
        if (parsed_line->label != NULL) {
            sym_add(parsed_line->label, DATA, LOC_UNK);
        }
        if (strcmp(parsed_line->directive, ".data") == 0) {
            for (i_arg = 0; i_arg < parsed_line->num_of_args; i_arg++) {
                data_to_stack(int_val(parsed_line->args[i_arg], 0));
            }
        }
        else { /* string data: need to convert it to a seq of ascii values (excluding the quotes) */
            for (i = 1; i < strlen(parsed_line->args[0]) -1; i++) {
                data_to_stack((int)parsed_line->args[0][i]);
            }
            data_to_stack(0); /* terminating 0 */
        }
    }
    else if (strcmp(parsed_line->directive, ".extern") == 0) {
        sym_add(parsed_line->args[0], Linker_UNK, LOC_EXTERNAL);
    }
}
