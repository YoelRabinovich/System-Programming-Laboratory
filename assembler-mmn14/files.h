#ifndef FILES_H
#define FILES_H

#include <stdio.h>

/* return filename */
char* file_name(char* base, char* extension);

/* writes a number to file (in padded hex format) */
void num_to_file(FILE *fp, int val);

/* writes an address to file (in padded hex format) */
void address_to_file(FILE *fp, int val);

#endif
