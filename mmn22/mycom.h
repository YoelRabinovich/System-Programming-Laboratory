#ifndef MYCOM_H_
#define MYCOM_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define EXIT           0
#define EQAULS         0


enum State        {COMMAND, VARIABLE, PARAMETER, ERROR};
enum parameter       {TWO_COMPLEX, TWO_NUMBERS, COMPLEX_AND_NUMBER, NONE};

void clearBuffer();
int stateAsCmd(int indexOfCmd);
void clearCommandString(char *command);

#define MAX_COMMAND     30
#define MIN_COMMAND     4
#define MAX_SCALAR      4
#define ENTER_KEY       10
#define MIN_ARGS        3


#endif
