/*
 * yoel rabinovich
 * mmn22
 * complex.h
 */

#ifndef complex_H_
#define complex_H_


typedef struct complex{
    double real,img;
} complex;

extern complex A,B,C,D,E,F;


void read_comp(complex *c, double a, double b);
void print_comp(complex *c);
void add_comp(complex *c1, complex *c2);
void sub_comp(complex *c1, complex *c2);
void mult_comp_real(complex *c, double scalar);
void mult_comp_img(complex *c, double scalar);
void mult_comp_comp(complex *c1, complex *c2);
void abs_comp(complex *c);



#endif
