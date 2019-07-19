#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

bool assert_int_equal(int a, int b){
    return a == b;
}

bool assert_double_equal(double a, double b){
    char string1[10], string2[10];
    
    sprintf(string1, "%.3f", a);
    sprintf(string2, "%.3f", b);

    return !strcmp(string1, string2);
}

bool assert_double_similar(double a, double b, double max_difference){
    return fabs(a - b) < max_difference;
}