/*
 * yoel rabinovich
 * mmn22
 * complex.c
 */


#include "complex.h"
#include "mycom.h"

/*all functions in header file*/

void read_comp(complex *c, double real, double img)
{
    (*c).real = real;
    (*c).img = img;
}

void print_comp(complex *c)
{
    printf("Print Complex Number : \n");
    printf("%.2f + %.2fi\n", (*c).real, (*c).img);
}

void add_comp(complex *c1, complex *c2)
{
    complex temp;
    temp.real = (*c1).real + (*c2).real;
    temp.img = (*c1).img + (*c2).img;
    printf("The addition result :\n");
    print_comp(&temp);
}

void sub_comp(complex *c1, complex *c2)
{
    complex temp;
    temp.real = (*c1).real - (*c2).real;
    temp.img = (*c1).img - (*c2).img;

    printf("The subtraction result  :\n");
    print_comp(&temp);
}

void mult_comp_real(complex *c, double scalar)
{
    complex temp;
    temp.real = (*c).real * scalar;
    temp.img = (*c).img * scalar;

    printf("The multiplication with real number  :\n");
    print_comp(&temp);

}

void mult_comp_img(complex *c, double scalar)
{
    complex temp;
    temp.real = (*c).real * scalar;
    temp.img = -(*c).img * scalar;

    printf("The multiplication with imaginary number result:\n");
    if (temp.real < 0)
        printf("%.2f + (%.2fi)\n", temp.img, temp.real);
    else
        printf("%.2f + %.2fi\n", temp.img, temp.real);
}


void mult_comp_comp(complex *c1, complex *c2)
{
    complex temp;
    temp.real = ((*c1).real * (*c2).real) - ((*c1).img * (*c2).img);
    temp.img = ((*c1).real * (*c2).img) + ((*c1).img * (*c2).real);

    printf("The multiplication of two complex numbers result:\n");
    print_comp(&temp);
}


void abs_comp(complex *c)
{
    double absolue = sqrt( (pow((*c).real, 2)) + (pow((*c).img, 2)) );
    printf("The Absolute complex number value is: %.2f\n", absolue);
}







