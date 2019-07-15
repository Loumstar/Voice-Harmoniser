#include "../lightweight_complex.h"
#include <stdio.h>

int main(void){
    complex z1 = {62, 12}; // 1 + 2i
    complex z2 = {76, 93}; // 5 + i

    double_complex z4 = {1, 2};
    double_complex z5 = {5, 1};

    complex z3;
    double_complex z6;

    printf("1: Find the real and imaginary components of a complex number.\n");
    printf("    Assert equal 62 + 12i.\n");
    
    printf("        %u + %ui\n\n", creal(z1), cimag(z1));

    printf("2: Find the modulus of a complex number.\n");
/*
    printf("    Conjugate: Assert equal 1 + -2i.\n");

    cconj(z1, z3);
    printf("        %u + %ui\n\n", creal(z3), cimag(z3));
*/
    printf("    Modulus: Assert equal the square root of 3,988 = ~63.15.\n");
    
    printf("        %u...\n\n", cabs(z1));


    printf("3: Calculate new complex numbers with basic arithmetic.\n");
    printf("    Addition by real: Assert equal 65 + 12i.\n");

    cadd_by_real(z1, 3, z3);
    printf("        %u + %ui\n\n", creal(z3), cimag(z3));

    printf("    Addition: Assert equal 138 + 105i.\n");

    cadd(z1, z2, z3);
    printf("        %u + %ui\n\n", creal(z3), cimag(z3));

    printf("    Subtraction by real: Assert 60 + 12i.\n");

    csub_by_real(z1, 2, z3);
    printf("        %u + %ui\n\n", creal(z3), cimag(z3));

    printf("    Subtraction: Assert equal 14 + 81i.\n");

    csub(z2, z1, z3);
    printf("        %u + %ui\n\n", creal(z3), cimag(z3));

    printf("    Double Multiplication: Assert equal 3 + 11i.\n");

    dcmult(z4, z5, z6);
    printf("        %.0f + %.0fi\n\n", dcreal(z6), dcimag(z6));

    printf("    Division by real: Assert equals 31 + 6i.\n");

    cdiv_by_real(z1, 2, z3);
    printf("        %u + %ui\n\n", creal(z3), cimag(z3));
/*
    printf("    Division: Assert equal 0.269 + 0.346i.\n");
    
    cdiv(z1, z2, z3);
    printf("        %u + %ui\n\n", creal(z3), cimag(z3));
*/
    printf("    Double Exponential: Assert equal -1.131 + 2.472i.\n");
    
    dcexp(z4, z6);
    printf("        %.3f + %.3fi\n\n", dcreal(z6), dcimag(z6));

    return 0;
}