/*
Author: Yoel Rabinovich
File Name: sequence.c
*/

/*preprocessor */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_TEXT 100

void f_sequence(char str[]);

/*Main program that receives a character series as input and calls a function that returns the series type. */

int main () 
{
	char text[MAX_TEXT];

	printf("Please enter string:\n");

	scanf("%s",text);
	
	printf("the string %s is a  : ",text);

	f_sequence(text);

	return 0 ;

}

/* A function that gets as parameter a string and prints whether the string is : "really going up/down series" , "going up/down series" or "without order series". */

void f_sequence(char str[] )	

{		
	int len=strlen(str)-1;

	int i,same=0,big=0,small=0;

	for (i=0; i<len ;i++)
	{
		if (str[i]>str[i+1]){
			small ++;
		}
		else if (str[i]<str[i+1]){
			big ++;
		}
		else {
			same ++;
		}
	}

	
	if(same==len){
		
		printf(" going up series");
	}

	if(small==len){

		printf(" really going down series");
	}

	if(big==len){

		printf(" really going up series");
	}
	
	if(small==0 && big>0 && same>0){

		printf(" going up series");
	}
	
	if(big==0 && small>0 && same>0){

		printf(" going down series");
	}

	if(big>0 && small>0 ){

		printf(" without order series");
	}
	
}

	


	


	






