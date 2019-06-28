#include <math.h>

#define EULER M_E

/*
Small script to handle complex arithmetic to be used on the arduino.
*/

typedef double complex[2];

double creal(const complex z){
    return z[0];
}

double cimag(const complex z){
    return z[1];
}

double cabs(const complex z){
    return hypot(creal(z), cimag(z));
}

void cconj(const complex z, complex target){
    target[0] = creal(z);
    target[1] = -cimag(z);
}

void cadd_by_real(const complex z1, double a, complex target){
    target[0] = creal(z1) + a;
    target[1] = cimag(z1);
}

void cadd(const complex z1, const complex z2, complex target){
    target[0] = creal(z1) + creal(z2);
    target[1] = cimag(z1) + cimag(z2);
}

void csub_by_real(const complex z1, double a, complex target){
    target[0] = creal(z1) - a;
    target[1] = cimag(z1);
}

void csub(const complex z1, const complex z2, complex target){
    target[0] = creal(z1) - creal(z2);
    target[1] = cimag(z1) - cimag(z2);
}

void cmult(const complex z1, const complex z2, complex target){
    complex z3 = { //z3 is used so that if one of the complex numbers is also the target, the calculation is not affected.
        (creal(z1) * creal(z2)) - (cimag(z1) * cimag(z2)),
        (creal(z1) * cimag(z2)) + (cimag(z1) * creal(z2))
    };
    target[0] = z3[0];
    target[1] = z3[1];
}

void cdiv_by_real(const complex z1, double a, complex target){
    target[0] = creal(z1) / a;
    target[1] = cimag(z1) / a;
}

void cdiv(const complex z1, const complex z2, complex target){
    /*
    z1 / z2 
    = (a+bi) / (c+di) 
    = (a+bi)(c-di) / (c+di)(c-di)
    = (a+bi)(c-di) / (c2+d2)
    = z1 * conj(z2) / hypot(z2)^2
    */
    complex z3;
    cconj(z2, z3);
    cmult(z1, z3, z3);
    cdiv_by_real(z3, pow(cabs(z2), 2), target);
}

void cexp(const complex z, complex target){
    complex z2 = { //z2 is used so that if one of the complex numbers is also the target, the calculation is not affected.
        pow(EULER, creal(z)) * cos(cimag(z)),
        pow(EULER, creal(z)) * sin(cimag(z))
    };
    target[0] = z2[0];
    target[1] = z2[1];
}