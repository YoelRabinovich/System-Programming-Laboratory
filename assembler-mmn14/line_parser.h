#ifndef PARSER_H
#define PARSER_H

#include "assembler.h"

#define MAX_LABEL_LEN 31

/* ParsedLine maker */
ParsedLine* form_parsed(char* label, char* op, char* directive, int n_args, char** args);

/* Free memory allocated to a ParsedLine */
 void parsed_free(ParsedLine* parsed_line);

/* Total number of instruction/operand words that will be required to encode this line */
 int num_of_code(ParsedLine* line);

/* Total of data words that will be required to encode this line  */
int num_of_data(ParsedLine* line);

/* Total symbol declarations in this line */
int num_of_sym(ParsedLine* line);

/* Total symbol references in the current line */
int how_many_refs(ParsedLine* line);

/* Parses, checks syntax is according to specification, and restructures each input line */
ParsedLine* parsed_again(int line_number, char* line);

#endif
