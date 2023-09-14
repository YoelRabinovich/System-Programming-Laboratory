/*
Author: Yoel Rabinovich
File Name: sequence.c
*/

/*preprocessor */

#include <stdio.h>
#define N 3
#define TRUE 1
#define FALSE 0

typedef int adjmat [N][N];
int path(adjmat,int u,int v);

/*Main program which defines an instance of a matrix that shows whether a directed path exists from node U to node V in the tree represented
*/

int main() 
{
	adjmat mat;
	int i,j,u,v,result ; 
	
	/* Stored values into the 2D array*/
	for (i=0;i<N;i++)
	{
		for (j=0;j<N;j++)
		{
			printf("Input %d elements of the matrix (0 or 1) :\n",N*N);
			scanf("%d", &mat[j][i]);
		}
	}

    /* print the 2D array*/
	printf("\nThe matrix is : \n");
  	for(i=0;i<N;i++)
  	{
   	   printf("\n");
     	   for(j=0;j<N;j++)
	   {
           	printf("%d\t",mat[i][j]);
  	   }
 	printf("\n\n");
	}

    	/*while ((scanf("%d%d",&u,&v)==2)&&(u!=EOF)&&(v!=EOF))*/
	while ((u!=-1 && v!=-1) || (!EOF))
	{
	    printf("\nPlease enter 2 index  ( for exit enter -1 -1 ) : \n");
	    scanf("%d%d",&u,&v);
	    result= path(mat,u,v);
	
	if (result == FALSE)
        printf("the index are : %d and %d FALSE " ,u,v );	
	if (result == TRUE)
        printf("the index are : %d and %d TRUE " ,u,v );	
        
     }
		 
	return 0;	
}
	
/* A function that gets an adjamt type matrix and 2 nodes indexes Which returns true if a path exists and returns another false*/
	
int path(adjmat mat,int u,int v)
{
    int i,cnt,temp=v;
    
    if(u==v && u<=N && u>=0)
	 {
        return TRUE;
	 }
	 
	 if(u>N || v>N || u<0 || v<0)
	 {
	    return FALSE;
     }
     
     if(mat[u][v]==TRUE)
	 {
	    	return TRUE;
	 }
    
    
    while (temp!=-1)
    {
        cnt=0;
        
        for (i=0;i<N;i++)
        {   
            if (mat[i][temp]==TRUE)
            {
                temp=i;
                cnt++; 
            }   
            
            if (temp==u)
            {
             return TRUE; 
            }
         }
            
         if (cnt==0)  
         {
              temp=-1;
         }             
     }
    
    if (temp==-1)
    {
        return FALSE;
    }

    
	return FALSE ; 
}



	

