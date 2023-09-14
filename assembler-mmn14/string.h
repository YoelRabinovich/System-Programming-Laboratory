#ifndef STRING_H
#define STRING_H

/* Cuting whitespace from str*/
char * cut(char* str);

/* How many times character occurs in str*/
int chars_in_string(char* str, char c);

/* checks if the str is alphabetic*/
int check_alpha(char* str);

/* checks if the str is alphanumeric*/
int check_alnum(char* str);

/* chekcs if the str is a valid string representation of an integer*/
int check_int(char* str, int start_idx);

/* Converts a integer string */
int int_val(char * str, int start_idx);

/* chekcs if the str is printable */
int check_print(char* str);

/* Replace a section of a string with the specified replacement */
char* rep_string(char* str, char* replacement, int start_idx, int end_idx);

/* Copy a str */
char* copy_str(char* str);

/* Return substring */
char* sub_get(char* str, int start_idx, int end_idx);

#endif
