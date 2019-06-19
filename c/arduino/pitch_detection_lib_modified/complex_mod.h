#include <math.h>

#define EULER M_E

typedef double complex[2];

double creal(complex z[]){
    return *z[0];
}

double cimag(complex z[]){
    return *z[1];
}

double cabs(complex z[]){
    return hypot(creal(z), cimag(z));
}

complex* cconj(complex z[]){
    complex res = {
        creal(z),
        -cimag(z)
    };
    return &res;
}

complex* cadd(complex z1[], complex z2[]){
    complex res =  {
        creal(z1) + creal(z2),
        cimag(z1) + cimag(z2)
    };
    return &res;
}

complex* csub(complex z1[], complex z2[]){
    complex res = {
        creal(z1) - creal(z2),
        cimag(z1) - cimag(z2)
    };
    return &res;
}

complex* cmult(complex z1[], complex z2[]){
    complex res = {
        (creal(z1) * creal(z2)) - (cimag(z1) * cimag(z2)),
        (creal(z1) * cimag(z2)) + (cimag(z1) * creal(z2))
    };
    return &res;
}

complex* _cdivide_by_real_number(complex z1[], double a){
    complex res = {
        creal(z1) / a,
        cimag(z1) / a
    };
    return &res;
}

 div(complex z1[], complex z2[]){
    complex* res = _cdivide_by_real_number(
        cmult(z1, cconj(z2)),
        pow(cabs(z2), 2)
    );
    return &res;
}

complex* cexp(complex z[]){
    complex res = {
        pow(EULER, creal(z)) * cos(cimag(z)),
        pow(EULER, creal(z)) * sin(cimag(z)),
    };
    return &res;
}

/*
class complex {
    public:
        double real;
        double imag;

        void set(double r, double i){
            real = r; 
            imag = i;
        };

        double abs(){
            return hypot(real, imag);
        };

        bool operator == (const complex&);
        bool operator != (const complex&);

        complex operator - (); // negation

        complex operator + (const complex&);
        complex operator - (const complex&);
        complex operator * (const complex&);
        complex operator / (const complex&);

        complex& operator += (const complex&);
        complex& operator -= (const complex&);
        complex& operator *= (const complex&);
        complex& operator /= (const complex&);

}

bool complex::operator == (const complex &c)
{
    return (real == c.real) && (imag == c.imag);
}

bool complex::operator != (const complex &c)
{
    return (real != c.real) || (imag != c.imag);
}

// NEGATE
complex* complex::operator - ()
{
    return complex(-real, -imag);
}

// BASIC MATH
complex* complex::operator + (const complex &c)
{
    return complex(real + c.real, imag + c.imag);
}

complex* complex::operator - (const complex &c)
{
    return complex(real - c.real, imag - c.imag);
}

complex* complex::operator * (const complex &c)
{
    double r = real * c.real - imag * c.imag;
    double i = real * c.imag + imag * c.real;
    return complex(r, i);	
}

complex* complex::operator / (const complex &c)
{
    double f = (c.real * c.real) + (c.imag * c.imag);
    double r = (real * c.real + imag * c.imag) / f;
    double i = (imag * c.real - real * c.imag) / f;
    return complex(r, i);
}

complex& complex::operator += (const complex &c)
{
    real += c.real;
    imag += c.imag;
    return *this;
}

complex& complex::operator -= (const complex &c)
{
    real -= c.real;
    imag -= c.imag;
    return *this;
}

complex& complex::operator *= (const complex &c)
{
    real = (real * c.real) - (imag * c.imag);
    imag = (real * c.imag) + (imag * c.real);
    return *this;
}

complex& complex::operator /= (const complex &c)
{
    double denom = (c.real * c.real) + (c.imag * c.imag);
    real = (real * c.real + imag * c.imag) / denom;
    imag = (imag * c.real - real * c.imag) / denom;
    return *this;
}
*/