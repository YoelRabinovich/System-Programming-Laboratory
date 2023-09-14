#include <stdlib.h>
#include <string.h>

#include "files.h"

/* return filename with extension */
char* file_name(char* base, char* extension) {
    char* filename = (char *)malloc(strlen(base) + strlen(extension) + 1);
    if (filename) {
        strcpy(filename, base);
        strcat(filename, extension);
    }
    return filename;
}

/* writes a number to file (in padded hex format) */
void num_to_file(FILE *fp, int val) {
    fprintf(fp, "%06x\n", val);
}

/* writes an address to file (in padded hex format) */
void address_to_file(FILE *fp, int val) {
    fprintf(fp, "%07d ", val);
}
