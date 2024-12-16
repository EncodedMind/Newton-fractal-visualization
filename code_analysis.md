# Code Analysis

This task consists of three parts: First, we need to implement basic arithmetic operations for complex numbers. Then, we calculate the roots of the polynomial for each initial value z<sub>0</sub> in the given range. Finally, we assign each root a unique color to create the pixel array for the BMP image and generate the Newton fractal for the specific polynomial.

## Arithmetic Operations on Complex Numbers
First, we used the structure to represent any complex number as a combination of a real and an imaginary part.
```c
typedef struct {
    double real;
    double imag;
} complex;
```
Next, we implemented the basic arithmetic operations (addition, subtraction, multiplication, and division) which we assumed would be necessary for calculating the roots. The definitions were sourced from the relevant Wikipedia article (<a href="https://en.wikipedia.org/wiki/Complex_number">Complex Numbers</a>).
<br>
It is worth noting that the implementation of multiplication differs depending on whether it involves a real and a complex number (`multiply_real`) or two complex numbers (`multiply_complex`). <br>
However, while creating the main program, we realized that these functions were not sufficient!<br>
Specifically, we needed to calculate the power of a complex number (raised to an integer exponent) to determine the values of f(z<sub>n</sub>) and f'(z<sub>n</sub>), as well as the magnitude of a complex number to compare the deviation between two successive approximations against 10<sup>-6</sup> and determine if the algorithm has sufficiently converged.
Finally, we included the definitions of these functions, along with the declaration of the `complex` structure, in the header file `complexlib.h` so they could be accessed in the main part of our program using the `#include` directive.

## Calculating the Polynomial Roots

1) **Reading the input file, storing data, and calculating derivative coefficients** <br>
As a first step, the `main` function opens the file provided as an argument and reads \( n \), the degree of the polynomial. It then dynamically allocates memory and stores the polynomial coefficients in the `coefficients` array of size \( n+1 \).<br>
Next, memory is allocated for the derivative coefficients (`der_coefficients`), which are calculated using the formula:<br><br>
f(x) = a<sub>n</sub>x<sup>n</sup>+a<sub>n-1</sub>x<sup>n-1</sup>+...+a<sub>1</sub>x+a<sub>0</sub> => f'(x) = na<sub>n</sub>x<sup>n-1</sup>+(n-1)a<sub>n-1</sub>x<sup>n-2</sup>+...+2a<sub>2</sub>x+a<sub>1</sub><br><br>
The bounds of the search range for both the real and imaginary parts are then read by the program and stored in the corresponding arrays (`real_span` and `imag_span`).
<br>

At this point, we should add that if, for any reason, reading the data fails (e.g., the `scanf` function does not return the expected number of elements), the program terminates using the `handle_read_error` function, printing the relevant error message to `stderr`.
<br><br>

2) **Finding and printing solutions - the `find_better_approx` function** <br>
Similar to the corresponding exercise in Assignment 1, approximations for the roots of the polynomial are found by calling the recursive `find_better_approx` function with an initial z<sub>0</sub>, each complex number in the range defined by the input data.<br>
The function calculates the ratio f(z<sub>n</sub>) / f'(z<sub>n</sub>):
```c
complex fraction = divide(find_f_of_z(z_n, coefficients, n+1), find_f_of_z(z_n, der_coefficients, n));
```
and then the next (better) root approximation (z<sub>n+1</sub>):
```c
complex next_z = subtract(z_n, fraction);
```
The recursion terminates in any of the following cases:<br>
1. The function has been called 1000 times<br>
This is checked using the `calls` argument, which is initialized to 1 and incremented by 1 with each call.<br>
2. z<sub>n+1</sub> is of type `nan`: the algorithm diverges<br>
3. The deviation between z<sub>n+1</sub> and z<sub>n</sub> (the magnitude of their difference) is less than 10<sup>-6</sup>: the algorithm has sufficiently converged.
<br>

If none of the above are true, the function calls itself with the new argument z<sub>n+1</sub> to find a better approximation of the solution.
<br>
Otherwise, it prints the corresponding result to the output, and program control returns to the `main` function.

3) **The helper function `find_f_of_z`**<br>
The function:
```c
complex find_f_of_z(complex z, double *coefficients, int length);
```
takes as arguments a complex number z, a coefficients array, and its length, and returns the value of the polynomial of degree length+1 defined by the given coefficients, for the value \( z \).<br>
This allows us to calculate both f(z) and f'(z) by passing the appropriate arguments each time.

## From Printing Solutions to Creating the Fractal
1. **Procedures: the `find_dimensions` and `write_header` functions**<br>
The `find_dimensions` function assigns values to the global variables `width`, `height`, and `padding`, determining the exact dimensions of the image's pixel array. Specifically, it sets the width equal to the number of distinct values for the imaginary parts in the search range (the number of columns in the solutions matrix):<br>
```c
width = (imag_span[1] - imag_span[0]) / step + 1;
```
Similarly, the height equals the number of rows in the solutions matrix:
```c
height = (real_span[1] - real_span[0]) / step + 1;
```
Padding is added at the end of each row of the pixel array so that the total number of bytes in each row is a multiple of 4:
```c
padding = (4 - (3*width) % 4) % 4;
```

Following the <a href="https://en.wikipedia.org/wiki/BMP_file_format">BMP file format standard</a>, we created the `write_header` function to write the necessary data to the output file for the correct display of our image.<br>
The comments in the program thoroughly explain what each group of printed bytes corresponds to. However, it is worth noting that for certain fields (e.g., color planes and resolution), we arbitrarily chose values commonly used in images of this type.<br>

2. **Creating the pixel array - the `paint` function**<br>
Each time the `find_better_approx` function finds a satisfactory approximation or ends with `nan` or `incomplete`, the `paint` function is called to render the result on the pixel array.<br>
Specifically, it first checks if the same result has already been found and thus has already been assigned a color.
* In this case, the `red`, `blue`, and `green` (RGB) values are set to those previously assigned to the specific result using the `search` function.
* If it is a new result, new values are assigned to `red`, `green`, and `blue`, randomly determined using `rand()`. These values, along with the result they correspond to, form a new `color_pair`, which is added to the list using the `push` function.<br>
Next, the values are written to the output file (in reverse order, since the pixel bytes are not in the order `rgb` but `bgr`).<br>
Finally, the static variable `pixels_in_row` is incremented by 1, and it is checked whether it equals the image width. If it does, this means a row of pixels has been completed, so padding is added, and `pixels_in_row` is reset to 0.

3. **Storing `color_pairs` - the helper functions `search`, `push`, and `free_color_pairs`**
<br>

Each color-result mapping is stored in a `color_pair` structure:
```c
typedef struct color_pair {
    complex number;
    char nan;
    char incomplete;
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    struct color_pair* next;
} color_pair;
```
This structure stores the result corresponding to a specific color combination. Since the `color_pairs` collection is organized as a singly linked list, the structure declaration includes a `next` pointer to the next element in the list.<br>
The `search` function checks if the result has already been added to the list, while the `push` function adds a new `color_pair` to the list, taking as arguments the values for each field of the structure.<br>
Finally, the `free_color_pairs` function is called just before the program terminates, ensuring that the memory allocated for the `color_pairs` is deallocated.
