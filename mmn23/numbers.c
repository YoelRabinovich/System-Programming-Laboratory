#include <stdio.h>
#include <stdlib.h>
  /*------------------errors-------------------------*/
    const char longInput[] = "To many arguments you can enter maximum 2 files";
    const char InputNotFound[] = "input file was not found / had a problem to open ";
    const char OutputNotFound[] = "output file was not found / had a problem to open ";

    /*------------------numbers-------------------------*/

    char *single[] = {""/* Filler */, "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
    char *dual[] = {"", ""/* Filler */, "twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"};
    char *tens[] = {"ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen","nineteen"};


   int print_number (int argc, char *argv[]);

    int number;
    FILE *input;
    FILE *output;


int main(int argc, char *argv[]) {

/* ---- too many files----*/
    if (argc > 3) {
        fprintf(stderr, "ERROR - %s\n", longInput);
        exit(0);
    }
    return print_number (argc, argv);
	return 0;
} /*main end*/



/*-------------------------------print number function------------------------------------------- */

/*(option 1) if 1 file , checking this file if opens and sending the output to the standard output(stdout) */

	int print_number (int argc, char *argv[]){
	 if (argc == 2) {
        input = fopen(argv[1], "r");
        if (!input ) {
            fprintf(stderr, "ERROR - %s\n", InputNotFound);
            exit(0);
        }
        while (fscanf(input, "%d", &number) > 0) {
            switch (number / 10) {
                case 0:
                    break;
                case 1:
                    printf("%s\n", tens[number % 10]);
	            continue;
                default:
                    printf("%s ", dual[number / 10]);
            }
            printf("%s\n", (number) ? single[number % 10] : "zero"); 
        }
    }
/*(option 2) if 2 files , checking them if opens and sending the output to the second file (output file) */

     if (argc == 3)  {
        output = fopen(argv[2], "w");
        if (!output ) {
            fprintf(stderr, "ERROR - %s\n", OutputNotFound);
            exit(0);
        }
        while (fscanf(input, "%d", &number) > 0) {
            switch (number / 10) {
                case 0:
                    break;
                case 1:
                    fprintf(output,"%s\n", tens[number % 10]);
	            continue;
                default:
                    fprintf(output,"%s ", dual[number / 10]);
            }
            fprintf(output,"%s\n", (number) ? single[number % 10] : "zero"); 
        }
    }

 /*(option 0) if 0 files , taking the input from the standard input(stdin) sending the output to the standard output(stdout) */
    
	if (argc == 1)  { 
        while (scanf("%d", &number) != EOF ) 
	{            
            switch (number / 10) 
	    {
                case 0:
                    break;
                case 1:
                    printf("%s\n", tens[number % 10]);
	            continue;
                default:
                    printf("%s ", dual[number / 10]);
            }
            printf("%s\n", (number) ? single[number % 10] : "zero"); 
        }
    } 
return 0;
}/*end print function*/


