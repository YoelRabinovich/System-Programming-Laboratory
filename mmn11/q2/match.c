/*
Author: Yoel Rabinovich
File Name: match.c
*/

/*preprocessor */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_TEXT 100

int string_matching(char p[] ,char t[]);

/* A main program that receives two strings from the user and calls the function "string_matching". 
   The program prints both strings and the position that gives the best match. */

int main () 
{
	int bestpos; 	
	
	char pattern[MAX_TEXT];
	
	char text[MAX_TEXT];

	printf("Please enter pattern \n");

	gets(pattern);
	
	printf(" the pattern is %s \n" , pattern);

	printf("Please enter text \n");
	
	gets(text);

	printf("the text is %s \n " , text);
	
	bestpos = string_matching(pattern , text);

	printf("and the best matched position is %d " , bestpos);

	return 0 ;

}

/* The function receives pattern and text and returns the position in the text which gives the best match */

int string_matching(char p[] , char t[])	

{		
	int tlen=strlen(t);

	int plen=strlen(p);

	int tmp, min, pos ; 
	
	min =plen; 

	int i, j;

	for (i = 0; i <= (tlen-plen) ; i++) { /* loop until the end of the text minus the length of the pattern */

		tmp=0;

		for (j = 0 ; j < plen; j++) { /* loop until the end of the pattern*/
		
			if (t[j+i]!=p[j]){   /* checks If the character in the text is different from the character in the pattern -> updates temporary amount*/
				tmp ++;
			}
		}
		
		if (tmp<min){ /* if the amount of different characters is lower -> updates min -> updates best matched positon  */
			min=tmp;
			
			pos=i;
		}
	}
	
	return pos;

}


	


	


	






