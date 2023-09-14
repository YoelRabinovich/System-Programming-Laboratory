#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "string.h"

/* Cuting whitespace from str*/
char * cut(char* str) {
    char* start;
    char* end;

    if (str == NULL)
        return str;
    start = str;
    end = start + strlen(str) - 1;
    while(*start && isspace(*start))
        start++;
    while(end > start && isspace(*end))
        *end-- = '\0';
    return start;
}

/* How many times character occurs in str*/
int chars_in_string(char* str, char c) {
    char* ptr;
    int count;

    if (str == NULL) {
        return 0;
    }

    ptr = str;
    count = 0;
    while(*ptr) {
        if (*ptr++ == c)
            count++;
    }
    return count;
}

/* checks if the str is alphabetic*/
int check_alpha(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (!isalpha(str[i])) {
            return 0;
        }
    }
    return 1;
}

/* checks if the str is alphanumeric*/
int check_alnum(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (!isalnum(str[i])) {
            return 0;
        }
    }
    return 1;
}

/* checks if the str is a valid string representation of an integer*/
int check_int(char* str, int start_idx) {
    int i;
    for (i = start_idx; i < strlen(str); i++) {
        if (i == start_idx && (str[i] == '+' || str[i] == '-')) {
            continue;
        }
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

/* Converts a integer string */

int int_val(char * str, int start_idx) {
    int value;
    int i;
    int sign;
    int power_of_10;

    if (!check_int(str, start_idx)) {
        return 0;
    }

    /* iterate backwards in increasing powers of 10 */
    value = 0;
    power_of_10 = 1;
    sign = 1;
    for (i = strlen(str) - 1; i >= start_idx; i--) {
        if (i == start_idx && (str[i] == '+' || str[i] == '-')) {
            if (str[i] == '-') {
                sign = -1;
            }
        }
        else {
            value += (str[i] - '0') * power_of_10; /* ascii to number */
            power_of_10 *= 10;
        }
    }
    return value * sign;
}

/* checks if the str is printable */
int check_print(char* str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        if (!isprint(str[i])) {
            return 0;
        }
    }
    return 1;
}

/* Replace a section of a string with the specified replacement . */
char* rep_string(char* str, char* replacement, int start_idx, int end_idx) {
    return strcat(strcat(sub_get(str, 0, start_idx), replacement), sub_get(str, end_idx, strlen(str)));
}

/* Copy a str. (should free when done) */
char* copy_str(char* str) {
    char* dest;
    if (str == NULL) {
        return NULL;
    }
    dest = (char*)malloc(sizeof(char) * (strlen(str) + 1));
    if (dest == NULL) {
        return NULL;
    }
    return strcpy(dest, str);
}

/* Return substring */
char* sub_get(char* str, int start_idx, int end_idx) {
    int i;
    int j;
    char* dest;
    if (str == NULL) {
        return NULL;
    }
    dest = (char*)malloc(sizeof(char)*(end_idx-start_idx)+1);
    if (dest == NULL) {
        return NULL;
    }
    for (i = start_idx, j =0; i < end_idx; i++, j++) {
        dest[j] = str[i];
    }
    dest[end_idx-start_idx] = '\0';
    return dest;
}


