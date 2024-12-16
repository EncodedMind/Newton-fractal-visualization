#include <stdio.h>
#include <math.h>
#include "complexlib.h"
complex add(complex a, complex b) {
    complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

complex substract(complex a, complex b) {
    complex result;
    result.real = a.real - b.real;
    result.imag = a.imag - b.imag;
    return result;
}

complex multiply_real(double a, complex b) {
    complex result;
    result.real = a * b.real;
    result.imag = a * b.imag;
    return result;
}

complex multiply_complex(complex a, complex b) {
    complex result;
    double u = b.real;
    double x = a.real;
    double v = b.imag;
    double y = a.imag;
    result.real = x*u-y*v;
    result.imag = x*v+y*u;
    return result;
}

complex divide(complex a, complex b) {
    complex result;
    double u = a.real;
    double x = b.real;
    double v = a.imag;
    double y = b.imag;
    result.real = (u*x + v*y) / (x *x + y*y);
    result.imag = (v *x - u*y) / (x*x + y*y);
    return result;
}

complex power(complex a, int power) {
    complex result = {1,0};
    for (int i = 0; i < power; i++) {
        result = multiply_complex(result, a);
    }
    return result;
}

double abs_complex(complex z) {
    double a = z.real;
    double b = z.imag;
    return sqrt(a*a+b*b);
}
