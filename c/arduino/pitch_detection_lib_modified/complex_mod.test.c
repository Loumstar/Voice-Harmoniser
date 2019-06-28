#include "complex_mod.h"
#include <stdio.h>

int main(void){
    complex z1 = {1, 2}; // 1 + 2i
    complex z2 = {5, 1}; // 5 + i
    complex z3;

    printf("1: Find the real and imaginary components of a complex number.\n");
    printf("    Assert equal 1 + 2i.\n");
    printf("        %.0f + %.0fi\n\n", creal(z1), cimag(z1));

    printf("2: Find the conjugate and modulus of a complex number.\n");
    printf("    Conjugate: Assert equal 1 + -2i.\n");
    cconj(z1, z3);
    printf("        %.0f + %.0fi\n", creal(z3), cimag(z3));
    
    printf("    Modulus: Assert equal the square root of 5 = ~2.236.\n");
    printf("        %.4f...\n\n", cabs(z1));


    printf("3: Calculate new complex numbers with basic arithmetic.\n");
    printf("    Addition by real: Assert equal 4 + 2i.\n");
    cadd_by_real(z1, 3, z3);
    printf("        %.0f + %.0fi\n", creal(z3), cimag(z3));

    printf("    Addition: Assert equal 6 + 3i.\n");
    cadd(z1, z2, z3);
    printf("        %.0f + %.0fi\n", creal(z3), cimag(z3));

    printf("    Subtraction by real: Assert -1 + 2i.\n");
    csub_by_real(z1, 2, z3);
    printf("        %.0f + %.0fi\n", creal(z3), cimag(z3));

    printf("    Subtraction: Assert equal -4 + 1i.\n");
    csub(z1, z2, z3);
    printf("        %.0f + %.0fi\n", creal(z3), cimag(z3));

    printf("    Multiplication: Assert equal 3 + 11i.\n");
    cmult(z1, z2, z3);
    printf("        %.0f + %.0fi\n", creal(z3), cimag(z3));

    printf("    Division by real: Assert equals 0.333 + 0.667i.\n");
    cdiv_by_real(z1, 3, z3);
    printf("        %.3f + %.3fi\n", creal(z3), cimag(z3));

    printf("    Division: Assert equal 0.269 + 0.346i.\n");
    cdiv(z1, z2, z3);
    printf("        %.3f + %.3fi\n", creal(z3), cimag(z3));

    printf("    Exponential: Assert equal -1.131 + 2.472i.\n");
    cexp(z1, z3);
    printf("        %.3f + %.3fi\n", creal(z3), cimag(z3));

    return 0;
}