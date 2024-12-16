# Newton Fractal Visualization

This repository contains a C implementation of the Newton-Raphson method for finding roots of polynomials in the complex plane. It also generates Newton fractals, providing a visual representation of how different starting points converge to polynomial roots.

## Repository Structure

```
├── src/
│   ├── complexlib.c       # Implementation of basic complex number operations
│   ├── complexlib.h       # Declarations for complex number functions
│   ├── fractal.c          # Main file for executing the program
├── test/
│   ├── input              # Sample input files for testing
│   ├── output             # Expected outputs for the input files
├── data/
│   ├── input              # Sample input files for testing
│   ├── output.bmp         # Expected output and visualization
├── README.md              # Documentation for the repository
```

## Features

- Implements Newton-Raphson method to find roots of polynomials.
- Generates Newton fractals in the complex plane.
- Highly modular code with reusable complex number operations.

## Getting Started

### Prerequisites

To build and run this project, you need:

- A C compiler (e.g., GCC)

### Building the Project

1. Clone this repository:
   ```
   git clone https://github.com/EncodedMind/newton-fractal-visualization.git
   cd newton-fractal-visualization
   ```

2. Compile the project:
   ```
   gcc -Ofast -Wall -Wextra -Werror -pedantic -c -o complexlib.o complexlib.c
   gcc -Ofast -Wall -Wextra -Werror -pedantic -c -o fractal.o fractal.c
   gcc -o fractal complexlib.o fractal.o -lm
   ```

3. Run the Program

1. Prepare an input file describing the polynomial and the desired grid size (see the `test/input` folder for examples).
2. Run the program:
   ```
   ./fractal input
   ```
   **If the graphical output is requested, then the `-g` flag must be present, followed by the output file: `output.bmp`. Ex.:**
   ```
   ./fractal input -g output.bmp
   ```

### Example Input File Format

```
Degree: 3
Coefficients: 1 0 -6 8
Grid: -2 2 -2 2 500
```

- `Degree`: Degree of the polynomial.
- `Coefficients`: Space-separated coefficients of the polynomial starting with the highest degree.
- `Grid`: Specifies the region of the complex plane to visualize (`xmin xmax ymin ymax resolution`).

## Testing

Sample input and output files are provided in the `test` directory for simple calculation, and `data` directory for graphical output. To verify correctness, compare program output with expected output.

## File Descriptions

### `complexlib.c` and `complexlib.h`

Contains the implementation of basic complex number operations, including:

- Addition, subtraction, multiplication, division.
- Magnitude and phase calculations.
- Polynomial evaluation in the complex plane.

### `fractal.c`

The main program file that:

- Reads input data.
- Executes the Newton-Raphson iteration.
- Generates the fractal image.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Acknowledgments

Inspired by the mathematical beauty of Newton fractals and their connection to complex dynamics.
This was a team project! Shoutout to my teammate: [Matina Nadali](https://github.com/matinanadali)
