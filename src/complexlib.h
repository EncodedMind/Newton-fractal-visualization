typedef struct {
double real;
double imag;
} complex;

complex add(complex a, complex b);
complex substract(complex a, complex b);
complex multiply_real(double a, complex b);
complex multiply_complex(complex a, complex b);
double abs_complex(complex a);
complex divide(complex a, complex b);
complex power(complex a, int power);
