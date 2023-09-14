#ifndef MACHINE_CODER_H
#define MACHINE_CODER_H

#include "assembler.h"

/*********************************** Function Prototypes ***********************************/

/* Initialize code image array:
 * returns 1 if success, 0 if failure */
int set_img_code(size_t n);

/* Initialize data image array:
 * returns 1 if success, 0 if failure */
int set_img_data(size_t n);

/* Initialize word_types array:
 * returns 1 if success, 0 if failure */
int set_type(size_t n);

/* Frees memory allocated for code image, data image and word_types array */
void memory_free();

/* Symbol table needs to know the current IC when adding new symbol */
unsigned int get_IC();

/* Symbol table needs to know the current DC when adding new symbol */
unsigned int get_DC();

/* Add an instruction word to the code stack */
void inst_adder(int opcode, addressing_type addrMode_1, int reg_1, addressing_type addrMod_2, int reg_2, int funct);

/* Add an operand word to the code stack */
void op_adder(int value, InfoLink linker_info);

/* Edit an operand word in the code image */
void op_edit(int ic, char*label, addressing_type mode);

/* Add a data word to the data stack */
void data_to_stack(int data);

/* Generate the machine code */
void ob_file(char* file_path);

/* Generate the .ext file */
void ext_file(char* file_path);

#endif
