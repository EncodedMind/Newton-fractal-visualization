#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "complexlib.h"
#define HEADER_SIZE 14 
#define DIB_HEADER_SIZE 40
#define RESOLUTION 2835

typedef struct color_pair {
    complex number;
    char nan;
    char incomplete;
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    struct color_pair*next;
} color_pair;

complex empty = {0,0};

int n;
unsigned int width, height, padding;

void handle_read_error(void);
void handle_memory_alloc_error(void);
void handle_file_error(void);

complex find_f_of_z(complex z, double *coefficients, int length);
void find_better_approx(complex z_n, int calls, double *coefficients, double *der_coeeficients, FILE *dest, color_pair**head);

void find_dimensions(double *real_span, double *imag_span, double step);
void write_header(FILE *dest);

void push(color_pair **head, complex number, int nan, int incomplete, unsigned int red, unsigned int green, unsigned int blue);
int search(color_pair *head, complex number,int nan, int incomplete, int *red, int *green, int *blue);

void paint(color_pair **head, complex number, int nan, int incomplete, FILE *dest);
void free_color_pairs(color_pair **head);

int main(int argc, char **argv) {
    srand((unsigned)time(NULL));
    if ((argc != 2 && argc != 4) || (argc == 4 && strcmp(argv[2], "-g"))) {
        perror("Invalid arguments.");
        exit(1);
    }
    
    FILE *in = fopen(argv[1], "r");
    if (!in) handle_file_error();

    FILE *image = NULL;
    color_pair *head = NULL;
    if (argc == 4) {
        image = fopen(argv[3], "wb+");
        if (!image) handle_file_error();
    }
    
    if (fscanf(in, "%d", &n) != 1) handle_read_error();
    
    
    double* coefficients = malloc((n+1) * sizeof(double));
    if (!coefficients) handle_memory_alloc_error();
    for (int i = 0; i <= n; i++) {
        if (fscanf(in, "%lf", coefficients+i) != 1) handle_read_error();
    }

    double*der_coefficients = malloc(n * sizeof(double));
    if (!der_coefficients) handle_memory_alloc_error();
    for (int i = 1; i < n+1; i++) {
        der_coefficients[i-1] = coefficients[i] * i;
    }

    double real_span[2];
    double imag_span[2];
    if (fscanf(in, "%lf %lf %lf %lf", real_span, imag_span, real_span+1, imag_span+1) != 4) handle_read_error();
    double step;
    if (fscanf(in, "%lf", &step) != 1) handle_read_error();

    if (image) {
        find_dimensions(real_span, imag_span, step);
        write_header(image);
    }
    
    for (double real = real_span[0]; real <= real_span[1]; real+=step) {
        for (double imag = imag_span[0]; imag <= imag_span[1]; imag+=step) {
            complex z = {real, imag};
            find_better_approx(z, 1, coefficients, der_coefficients, image, &head);
            printf(" ");
        }
        printf("\n");
    }

    free_color_pairs(&head);
    free(der_coefficients);
    free(coefficients);
    fclose(in);
    if (image)
    fclose(image);
    return 0;
}


complex find_f_of_z(complex z, double *coefficients, int length) {
    complex result = {0,0};
    for (int i = 0; i < length; i++) {
        complex new_term = multiply_real(coefficients[i], power(z,i));
        result = add(result, new_term);
    }
    return result;
}
void find_better_approx(complex z_n, int calls, double *coefficients, double *der_coefficients, FILE *image, color_pair**head) {
  if (calls == 1000) {
    printf("incomplete");
    paint(head, empty, 0,1,image);
    return;
  } else {
    complex fraction = divide(find_f_of_z(z_n, coefficients, n+1), find_f_of_z(z_n,der_coefficients,n));
    complex next_z = substract(z_n, fraction);

    if (isnan(next_z.real) || isnan(next_z.imag)) {
      printf("nan");
      paint(head, empty, 1,0,image);
      return;
    } else {
      if (abs_complex(substract(next_z, z_n)) < pow(10, -6)) {
        printf("%+.2f%+.2fi", next_z.real, next_z.imag);
        paint(head, next_z, 0,0,image);
        return;
      } else {
        find_better_approx(next_z, calls + 1, coefficients, der_coefficients, image, head);
      }
    }
  }
}

void write_header(FILE *image) {
    fputc('B', image);
    fputc('M', image);

    unsigned int size = (3*width+padding)*height + HEADER_SIZE+DIB_HEADER_SIZE;
    fwrite(&size, 4, 1, image);

    unsigned int zero = 0;
    fwrite(&zero,2, 2,image);

    unsigned int offset = HEADER_SIZE+DIB_HEADER_SIZE;
    fwrite(&offset, 4,1, image);

    unsigned int size_of_dib_header = DIB_HEADER_SIZE;
    fwrite(&size_of_dib_header, 4, 1, image);
    
    fwrite(&width, 4, 1, image);
    fwrite(&height, 4, 1, image);
    
    unsigned int color_planes = 1;
    fwrite(&color_planes, 2, 1, image);

    unsigned int bits_per_pixel = 24;
    fwrite(&bits_per_pixel, 2, 1, image);
    fwrite(&zero, 4, 1, image);
    
    unsigned int size_of_pixel_matrix = (3*width+padding)*height;
    fwrite(&size_of_pixel_matrix, 4, 1, image);
    
    unsigned int resolution = RESOLUTION;
    fwrite(&resolution, 4, 1, image);
    fwrite(&resolution, 4, 1, image);
    fwrite(&zero, 2, 2, image);
    fwrite(&zero, 2, 2, image);
}
unsigned pixels_in_row = 0;
void paint(color_pair **head, complex number, int nan, int incomplete, FILE *image) {
    if (!image) return;
    int red;
    int green;
    int blue;
    
    int exists = search(*head, number, nan, incomplete, &red, &green, &blue);
    if (!exists) {
        red = rand() % 256;
        blue = rand() % 256;
        green = rand() % 256;
        push(head, number, nan, incomplete, red, green, blue);
    }

    fwrite(&blue, 1, 1, image);
    fwrite(&green, 1, 1, image);
    fwrite(&red, 1, 1, image);
    pixels_in_row++;

    if (pixels_in_row == width) {
        unsigned int zero = 0;
        fwrite(&zero, 2, padding, image);
        pixels_in_row = 0;
    }
}

void push(color_pair **head, complex number, int nan, int incomplete, unsigned int red, unsigned int green, unsigned int blue) {
    color_pair *new_color_pair = malloc(sizeof(color_pair));
    if (!new_color_pair) handle_memory_alloc_error();
    new_color_pair->number = number;
    new_color_pair->nan = nan;
    new_color_pair->incomplete = incomplete;
    new_color_pair->red = red;
    new_color_pair->green = green;
    new_color_pair->blue = blue;
    new_color_pair->next = NULL;

    if (*head == NULL) {
        *head = new_color_pair;
    } else {
        new_color_pair->next = *head;
        *head = new_color_pair;
    }
}
void find_dimensions(double *real_span, double *imag_span, double step) {
    height = (real_span[1] - real_span[0]) / step + 1;
    width = (imag_span[1] - imag_span[0]) / step + 1;
    padding = (4 - (3*width) % 4) % 4;
}

int search(color_pair *head, complex number,int nan, int incomplete, int *red,int *green,int *blue) {
    color_pair *ptr = head;
    double tolerance = 1e-2;
    while(ptr) {
        if (fabs((ptr->number).real - number.real) <= tolerance && 
        fabs((ptr->number).imag - number.imag) <= tolerance && ptr->nan == nan && ptr->incomplete == incomplete) {
            *red = ptr->red;
            *green = ptr->green;
            *blue = ptr->blue;
            return 1;
        }
        ptr = ptr->next;
    }
    return 0;
}

void free_color_pairs(color_pair **head) {
    color_pair *ptr = *head;
    while(ptr) {
        color_pair *next = ptr->next;
        free(ptr);
        ptr = next;
    }
}

void handle_read_error() {
    perror("Could not open or read file.");
    exit(1);
}

void handle_memory_alloc_error() {
  perror("Error: Memory allocation failed");
  exit(1);
}

void handle_file_error() {
    perror("Could not open file.");
    exit(1);
}