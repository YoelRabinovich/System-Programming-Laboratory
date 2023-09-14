#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "line_parser.h"
#include "string.h"
#include "passes.h"

/* Constructor for ParsedLine struct */
ParsedLine* form_parsed(char* label, char* op, char* directive, int n_args, char** args) {
    int i;
    ParsedLine* parsed_line;
    parsed_line = (ParsedLine*)malloc(sizeof(ParsedLine));
    parsed_line->number_line = line_number;
    parsed_line->label = label; /*str_cpy(label);*/
    parsed_line->op = copy_str(op);
    parsed_line->directive = copy_str(directive);
    parsed_line->args = (char**)malloc(sizeof(char*) *  n_args);
    for (i = 0; i < n_args; i++) {
        parsed_line->args[i] = copy_str(args[i]);
    }
    parsed_line->num_of_args = n_args;
    free(args);
    return parsed_line;
}

/* Free memory allocated to a ParsedLine */
void parsed_free(ParsedLine* parsed_line) {
    int i;
    if (parsed_line->label != NULL) {
        free(parsed_line->label);
    }
    if (parsed_line->op != NULL) {
        free(parsed_line->op);
    }
    if (parsed_line->directive != NULL) {
        free(parsed_line->directive);
    }
    for (i = 0; i < parsed_line->num_of_args; i++) {
        free(parsed_line->args[i]);
    }
    if (parsed_line->args != NULL) {
        free(parsed_line->args);
    }
    free(parsed_line);
}

/* Total number of instruction/operand words that will be required to encode this line */
int num_of_code(ParsedLine* line) {
    int num_words_code;
    int i;
    num_words_code = 0;
    if (line->op != NULL) {
        num_words_code++; /* 1 instruction word */
        for (i = 0; i < line->num_of_args; i++) {
            if (find_reg(line->args[i]) == -1) { /* only non-register operands generate words */
                num_words_code++;
            }
        }
        return num_words_code;
    }
    else { /* .entry and .extern directives don't generate words in the machine code output */
        return 0;
    }
}

/* Total of data words that will be required to encode this line  */
int num_of_data(ParsedLine* line) {
    if (line->directive != NULL && strcmp(line->directive, ".data") == 0) {
        return line->num_of_args; /* 1 word for each integer */
    }
    else if (line->directive != NULL && strcmp(line->directive, ".string") == 0) {
        return strlen(line->args[0]) -2 + 1; /* the number of chars, minus the quotes, plus the terminating '\0' */
    }
    else { /* .entry and .extern directives don't generate words in the machine code output */
        return 0;
    }
}

/* Total symbol declarations in this line */
int num_of_sym(ParsedLine* line) {
    int n_line_symbols;
    int is_label_declaration;

    n_line_symbols = 0;

    /* A symbol can come from either a lable preceding an op or a .data / .string directive,
     * or as the arg of a .extern directive.
     * Labels preceding an .extern or an .entry directive do not count (and are ignored) */
    is_label_declaration =
            (line->label != NULL &&
                (line->op != NULL ||
                (line->directive != NULL && (strcmp(line->directive, ".data") == 0 || strcmp(line->directive, ".string") == 0))));

    is_label_declaration |= (line->directive != NULL && strcmp(line->directive, ".extern") == 0);

    if (is_label_declaration) {
        n_line_symbols++;
    }
    /* Give warning for redundant label declaration */
    if (line->label != NULL &&
            (line->directive != NULL && (strcmp(line->directive, ".entry") == 0 || strcmp(line->directive, ".extern") == 0))) {
        printf("Warning in line %i. Ignoring redundant label \'%s\' in directive \'%s\' ...\n", line_number, line->label, line->directive);
    }

    return n_line_symbols;
}

/* Total symbol references in the current line */
int how_many_refs(ParsedLine* line) {
    int n_symbol_refs;
    int i;
    n_symbol_refs = 0;
    if (line->op != NULL) {
        for (i = 0; i < line->num_of_args; i++) {
            addressing_type mode = add_type(line->args[i]);
            if (mode == DIRECT || mode == RELATIVE) { /* these are label references */
                n_symbol_refs++;
            }
        }
    }
    return n_symbol_refs;
}

/* Returns 1 if valid label , 0 otherwise  */
int is_vaild_label(char* label) {
    if (!isalpha(label[0]) || !check_alnum(label)) {
        printf("Error in line %i: Invalid label: \'%s\' (labels must start with a letter and contain only letters and numbers)\n", line_number, label);
        errors_num++;
        return 0;
    }
    if (strlen(label) > MAX_LABEL_LEN) {
        printf("Error in line %i: Label exceeds max length (31): \'%s\'\n", line_number, label);
        errors_num++;
        return 0;
    }
    if (find_reg(label) != -1) {
        printf("Error in line %i: Invalid label: \'%s\' (register names are reserved)\n", line_number, label);
        errors_num++;
        return 0;
    }
    if (get_op(label) != NULL) {
        printf("Error in line %i: Invalid label: \'%s\' (op names are reserved)\n", line_number, label);
        errors_num++;
        return 0;
    }
    if (get_directive(label) != NULL) {
        printf("Error in line %i: Invalid label: \'%s\' (directive names are reserved)\n", line_number, label);
        errors_num++;
        return 0;
    }
    return 1;
}

/* Returns 1 if valid int arg, 0 otherwise  */
int is_vaild_int(char* arg, int start_idx) {
    if (!check_int(arg, start_idx)) {
        printf("Error in line %i: Invalid integer value: \'%s\'\n", line_number, arg);
        errors_num++;
        return 0;
    }
    return 1;
}

/* Returns 1 if valid string arg (for .string directive) , otherwise 0 */
int is_vaild_str(char* arg) {
    if (strlen(arg) < 2 || arg[0] != '"' || arg[strlen(arg)-1] != '"') {
        printf("Error in line %i: String missing quotes: %s\n", line_number, arg);
        errors_num++;
        return 0;
    }
    if (chars_in_string(arg, '"') > 2) { /* we don't allow this, nor do we support escape characters to allow this */
        printf("Error in line %i: Quotes found inside string : \'%s\'\n", line_number, arg);
        errors_num++;
        return 0;
    }
    if (!check_print(arg)) {
        printf("Error in line %i: Invalid string  \'%s\'. (must contain only printable chars)\n", line_number, arg);
        errors_num++;
        return 0;
    }
    return 1;
}

/* Returns 1 if valid directive and checks that its args are according to the specification ,0 otherwise  */
int is_vaild_direc(char* directive_name, char** args, int n_args ,int line_number) {
    int i;
    Directive* directive = get_directive(directive_name);
    if (directive == NULL) {
        printf("Error in line %i. Unrecognized directive: \'%s\'\n", line_number, directive_name);
        errors_num++;
        return 0;
    }

    if (n_args == 0 || n_args > directive->n_args) {
        printf("Error in line %i. Incorrect number of args for \'%s\' directive. Expected %i but got %i\n",
                line_number, directive_name, directive->n_args, n_args);
        errors_num++;
        return 0;
    }

    for (i = 0; i < n_args; i++) {
        char* arg = args[i];
        if ((directive->arg_type == LABEL && !is_vaild_label(arg)) ||
                (directive->arg_type == INT && !is_vaild_int(arg, 0)) ||
                (directive->arg_type == STRING && !is_vaild_str(arg))) {
            return 0;
        }
    }
    return 1;
}

/* Returns 1 if valid operand (addressing type and val) , 0 otherwise */
int is_vaild_op(char* op_name, char* operand, char* operand_name, int* valid_addr_modes) {
    char* substr;
    addressing_type mode = add_type(operand);
    if (!valid_addr_modes[mode]) {
        printf("Error in line: %i. %s operand \'%s\' of \'%s\'. Invalid addr mode: %s'\n",
                line_number, operand_name, operand, op_name, addr_mode_str(mode));
        errors_num++;
        return 0;
    }
    if ((mode == IMMEDIATE && !is_vaild_int(operand, 1)) ||
    (mode == DIRECT && !is_vaild_label(operand))) {
        return 0;
    }
    if (mode == RELATIVE) {
        substr = sub_get(operand, 1, strlen(operand));
        if (!is_vaild_label(substr)) {
            free(substr);
            return 0;
        }
        free(substr);
    }
    return 1;
}

/* Returns 1 if valid op and checks that its args (both the number of args and their addr modes) , 0 otherwise */
int opr_vail(char* op_name, char** args, int n_args) {
    Op* op = get_op(op_name);
    if (op == NULL) {
        printf("Error in line %i. Unrecognized op: \'%s\'\n", line_number, op_name);
        errors_num++;
        return 0;
    }

    if (n_args != op->n_args) { /* checking the number of args */
        printf("Error in line %i. Incorrect number of args for \'%s\'. Expected %i but got %i\n",
               line_number, op_name, op->n_args, n_args);
        errors_num++;
        return 0;
    }

    if (n_args >= 1) { /* check the last arg with arg_2_modes */
        if (!is_vaild_op(op_name, args[n_args-1], "Dest",  op->second_arg_type)) {
            return 0;
        }
        if (n_args == 2) { /* also check the first arg with arg_1_modes */
            if (!is_vaild_op(op_name, args[0], "Source", op->first_arg_type)) {
                return 0;
            }
        }
    }
    return 1;
}

/* Check the number of commas and their location */
int comma_check(char* arg_input_str) {
    int i;
    int comma_num;

    /* Check commas location */
    if ((arg_input_str[0] == ',' || arg_input_str[strlen(arg_input_str) - 1] == ',')) {
        return 0;
    }

    /* Check number of commas */
    comma_num = 0;
    for (i = 0; i < strlen(arg_input_str); i++) {
        if (arg_input_str[i] == ' ') { /* ignore spaces */
            continue;
        }
        if (arg_input_str[i] == ',') { /* a comma */
            comma_num++;
            if (comma_num > 1) {
                return 0;
            }
        }
        else { /* a arg character so reset the commas count */
            comma_num = 0;
        }
    }
    return 1;
}

/* Parses, checks syntax is according to specification, and restructures each input line */
ParsedLine* parsed_again(int line_number, char* line) {
    char* label = NULL;
    char* op = NULL;
    char* directive = NULL;
    int args_num;
    int commas_num;
    int wrong_commas;
    char** args;
    char* token;
    char* arg_next;
    char* inp_arg;
    int len_tok;

    args = (char**)malloc(sizeof(char*) * MAX_LEN); /* can't have more args than the max length of line! */

    /* Strip newline character and trim leading and trailing whitespaces */
    line[strcspn(line, "\n")] = 0;
    line = cut(line);

    /* Skip empty lines and comments */
    if (strlen(line) == 0 || line[0] == ';') {
        return NULL;
    }

    if (strlen(line) > MAX_LEN) {
        printf("Error in line: %i. Line exceeds max length of %i chars\n", line_number, MAX_LEN);
        errors_num++;
    }

    /* Get first token of line */
    token = strtok(line, " \t");

    /* See if it's a label */
    len_tok = strlen(token);
    if (token[len_tok - 1] == ':') {
        label = sub_get(token, 0, len_tok - 1);

        /* Validate the label */
        if (!is_vaild_label(label)) {
            return NULL;
        }

        /* Move on to next token of line */
        token = cut(strtok(NULL, " \t"));
    }

    /* A label by itself (with no op or directive) is an error: */
    if (token == NULL) {
        printf("Error in line %i. No op or directive given\n", line_number);
        errors_num++;
        return NULL;
    }

    /* Get args: */
    args_num = 0;
    wrong_commas = 0;
    inp_arg = cut(strtok(NULL, "")); /* the remaining part of the line are the args */
    commas_num = 0;
    if (inp_arg != NULL) {
        /* If we're expecting a string arg or we got a string arg, don't split by commas and spaces
         * (the string itself might contain these chars, which is valid for a string): */
        if (strcmp(token, ".string") == 0 || (inp_arg[0]=='"' && inp_arg[strlen(inp_arg)-1]=='"')) {
            args[args_num++] = inp_arg;
        }
        else { /* otherwise, read in comma separated list of args one by one (if any) */
            commas_num = chars_in_string(inp_arg, ',');
            wrong_commas = !comma_check(inp_arg);
            arg_next = cut(strtok(inp_arg, ", \t"));
            while (arg_next != NULL) {
                args[args_num++] = arg_next;
                arg_next = cut(strtok(NULL, ", \t"));
            }
        }
    }

    /* Check the type of the command (directive/op) and validate accordingly: */
    if (token[0] == '.') { /* directive */
        if (!is_vaild_direc(token, args, args_num, line_number)) {
            return NULL;
        }
        directive = token;
    }
    else { /* op */
        if (!opr_vail(token, args, args_num)) {
            return NULL;
        }
        op = token;
    }

    wrong_commas |= (args_num == 0 && commas_num != 0) || (args_num > 0 && commas_num != args_num - 1);
    if (wrong_commas) {
        printf("Error in line %i. Bad comma formatting (a SINGLE comma is required BETWEEN each argument)\n", line_number);
        errors_num++;
    }
    return form_parsed(label, op, directive, args_num, args);
}
