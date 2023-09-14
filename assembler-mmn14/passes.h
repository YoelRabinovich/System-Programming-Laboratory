#ifndef PASSES_H
#define PASSES_H

#include "assembler.h"

/* 
In the first transition the assembler builds the table of symbols and the skeleton of the memory image (instructions and data).
The assembler reads the source file line by line, and acts according to the type of line (instruction, prompt, or blank line / note).
*/
 void do_pass_one();

/*
In the second transition, the assembler completes using the symbol table the encoding of all the words in the set of instructions that have not yet been coded in the first transition
In our machine model these are additional information words of commands, which encode operand by a direct or relative addressing method.
The operand contains a symbol that is defined as internal or external, so in the field E, R, A the bit R or bit E, respectively, will be 1
*/
void do_pass_two();

/*
Generates instruction words and operand words for lines with an 'op' in the source code to go into the code image (Used in the first pass). 
Some info will be missing due to label references whose addresses have not yet been entered into the symbol table - will be filled in in the second pass .
*/
void operand_handler(ParsedLine *parsed_line);


/* Adds operand for (non-register) arg :  For IMMEDIATE adding the full information (Used in first pass) . For DIRECT and RELATIVE operands - placeholder,
 stored some other information on the side to be used in the second pass */
void handle_operand(char *arg, addressing_type addr_mod);

/* Detect AddrMode of input arg */
addressing_type add_type(char* str);


/* enter numerical and string data into data image and symbol table  and also extern symbols into the symbol table (Used in the first pass). */
void directive_handle(ParsedLine *parsed_line);

#endif