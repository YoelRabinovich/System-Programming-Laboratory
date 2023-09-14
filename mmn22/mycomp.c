/*
 * yoel rabinovich
 * mmn22
 * complex.c
 */

#include "complex.h"
#include "mycom.h"


complex A,B,C,D,E,F;

/*complex array*/
struct {
    char *name;
    complex *var;
}complexList[] = {
    {"A",&A},
    {"B",&B},
    {"C",&C},
    {"D",&D},
    {"E",&E},
    {"F",&F},
    {"0", NULL}
};

/*function array*/
struct {
    char *name;
    void (*func)();
    int funcParam;
}cmdList[] = {
    {"read_comp", read_comp, TWO_NUMBERS},
    {"print_comp", print_comp, NONE},
    {"add_comp", add_comp, TWO_COMPLEX},
    {"sub_comp", sub_comp, TWO_COMPLEX},
    {"mult_comp_real", mult_comp_real, COMPLEX_AND_NUMBER},
    {"mult_comp_img", mult_comp_img, COMPLEX_AND_NUMBER},
    {"mult_comp_comp", mult_comp_comp, TWO_COMPLEX},
    {"abs_comp", abs_comp, NONE},
    {"not_valid", NULL, NONE}
};


int main()
{
    char command[MAX_COMMAND];
    int state = COMMAND, paramState = NONE,
    funcIndex = 0, varIndex = 0;
    int i,c = 0;
    char ch;
    char *cmdName = "", *varName = "", *rest = "" , *end;


    for(;;){
        clearCommandString(command);
	printf("you can enter function name and parameters \n if you would like to exit enter 'stop'\n");
        printf("---->>>> ");
        /*Scaning until enter-key*/
        for(i = 0; c != ENTER_KEY; i++){
            c = getchar();
            if (c == ENTER_KEY) break;
            ch = c;
            command[i] = ch;
            if(i > MAX_COMMAND){
                printf("ERROR: undefined command (command too long)");
                state = ERROR;
                clearBuffer();
                break;
            }
        }

        /*if 'stop' entered-> checking -> stop running*/
        if (strcmp(command, "stop") == EQAULS){
		printf("press enter key to exit the program");
            {
                int decision = getchar();
                if (decision == ENTER_KEY)
                    break;
                else
                    state = ERROR;
            }
        }
        
        /*if( i < MIN_COMMAND){
            printf("ERROR: undefined command (command too short)");
            state = ERROR;
        }*/
        c = 0;

        /*find command*/

        if (state == COMMAND) {

            int commandLength, funcLength, varLength;
            commandLength = strlen(command);

            cmdName = strtok(command, " ");
            funcLength = strlen(cmdName);

            /*find function*/
            if (state != ERROR) {
                for (i = 0; cmdList[i].func != NULL; i++)
                    if (strcmp(cmdName, cmdList[i].name) == EQAULS)
                        break;

                if (cmdList[i].func == NULL) {
                    state = ERROR;
                    printf("ERROR: %s is undefined command\n", cmdName);
                } else {
                    state = VARIABLE;
                    funcIndex = i;
                }


                if ((state != ERROR) && (commandLength - funcLength >= 2)) {
                    varName = strtok(NULL, ",");
                    varLength = strlen(varName);

                    if (commandLength - funcLength - varLength >= 2)
                        rest = strtok(NULL, "");
                    else if(i!=1) {
                        state = ERROR;
                        printf("ERROR: Missing parameters\n");
                    }
                }
            }
        }

        /*find var*/

        if (state == VARIABLE) {
            for(i=0; complexList[i].var != NULL; i++){
               if(strcmp(varName, complexList[i].name) == EQAULS)
                   break;
            }
            if(complexList[i].var == NULL){
                state = COMMAND;
		printf("ERROR: %s is undefined complex variable\n",varName);
    
            }else{
                paramState = stateAsCmd(funcIndex);
                state = PARAMETER;
                varIndex = i;
            }
        }

           /*switch cases*/
            if(state == PARAMETER) {
                switch (paramState) {
                    case NONE: {
                        cmdList[funcIndex].func(complexList[varIndex].var);
                        break;
                    }

                    case TWO_NUMBERS: {
                        int restLength = strlen(rest);
                        double real, img;
                        if (restLength < MIN_ARGS) {
                            state = ERROR;
                            printf("ERROR: missing parameters \n");
                            break;
                        }

                        real = strtod(strtok(rest, ","),&end);
                        if (*end!='\0'){
                            state=ERROR;
                            printf("ERROR: Invaild parameter - not a number \n");
                            break;
                        }
                        img = strtod(strtok(NULL, ","),&end);
                        if (*end!='\0'){
                            state=ERROR;
                            printf("ERROR: Invaild parameter - not a number \n");
                            break;
                        }

                        cmdList[funcIndex].func(complexList[varIndex].var, real, img);
                        printf("Reading to %s  : %.2f and %.2f\n", complexList[varIndex].name, real, img);
                        break;
                    }

                    case TWO_COMPLEX: {
                        int len;
                        len = strlen(rest);
                        if (len > 1)
                            printf("ERROR: extraneus text after end of command \n");
                        else {
                            for (i = 0; complexList[i].var != NULL; i++) {
                                if (strcmp(rest, complexList[i].name) == EQAULS)
                                    break;
                            }
                            if (complexList[i].var == NULL)
                                printf("ERROR: %s is undefined complex variable\n", rest);
                            else
                                cmdList[funcIndex].func(complexList[varIndex].var, complexList[i].var);
                        }

                        break;
                    }

                    case COMPLEX_AND_NUMBER: {
                        int len;
                        len = strlen(rest);
                        if (len > MAX_SCALAR)
                            printf("ERROR: extraneus text after end of command \n");
                        else {
                            char isDigitFlag = rest[0];
                            if (!isDigitFlag) {
                                printf("scalar argument, inster real number please  \n");
                                break;
                            } else {
                                double num = atof(rest);
                                cmdList[funcIndex].func(complexList[varIndex].var, num);
                            }
                        }

                        break;


                    }/*end switch */

                }
            }

        varName = "";
        rest = "";
        state = COMMAND;

    }/*END FOREVER*/
	  return EXIT;
}

/*return number of state*/
int stateAsCmd(int indexOfcmd)
{
    return cmdList[indexOfcmd].funcParam;
}


/*clear all buffer for new input*/
void clearBuffer()
{
    int c;
    c = getchar();
    while(c)
        if(c == ENTER_KEY){
            return;
        }
        else
            c = getchar();
}

/*clear command string*/
void clearCommandString(char *command)
{
    int i ;

    for (i = 0; i < MAX_COMMAND; i++){
         command[i] = 0;
     }
}











