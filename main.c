#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "matrix.h"
#include "utils.h"

struct timespec start, end;

int main(int argc, char* argv[]) {
    srand(time(NULL));
    if (argc != 4) {
        fprintf(stderr, "Usage: ./prog [-r|-c|-s|-a] <size> <kernel> \n -r: Row-major convolution \n -c: Column-major convolution \n -s: SIMD convolution \n -a: All convolutions \n");
        fprintf(stderr, "Example: ./prog -r 1024 3\n");
        return 1;
    }

    if (strcmp(argv[1], "-r") != 0 &&
        strcmp(argv[1], "-c") != 0 &&
        strcmp(argv[1], "-s") != 0 &&
        strcmp(argv[1], "-a") != 0) {

            fprintf(stderr, "Usage: ./prog [-r|-c|-s|-a] <size> <kernel> \n -r: Row-major convolution \n -c: Column-major convolution \n -s: SIMD convolution \n -a: All convolutions \n");
            fprintf(stderr, "Example: ./prog -r 1024 3\n");
            return 1;
    }

    // Parse matrix and kernel sizes
    int n = atoi(argv[2]);
    int k = atoi(argv[3]);
    if (n <= 0 || k <= 0) {
        fprintf(stderr, "Matrix and kernel size must be positive integers.\n");
        return 1;
    }
    if (k > n) {
        fprintf(stderr, "Kernel size cannot be larger than matrix size.\n");
        return 1;
    }

    //Initialize matrices and kernels
    float** matrix = nxn(n);
    float** kernel = kxk(k);

    int out_size = n - k + 1;
    
    // Allocation de la matrice de sortie
    float** output = malloc(out_size * sizeof(float*));
    for (int i = 0; i < out_size; i++) {
        output[i] = malloc(out_size * sizeof(float));
    }
    clear_matrix(output, out_size);

    switch (argv[1][1]) {
        case 'r':
            printf("Benchmarking row-major convolution...\n");
            benchmark((void (*)(float**, float**, float**, int, int))convolve_row_major,
                      matrix, kernel, output, k, out_size);
            break;
        case 'c':
            printf("Benchmarking column-major convolution...\n");
            benchmark((void (*)(float**, float**, float**, int, int))convolve_col_major,
                      matrix, kernel, output, k, out_size);
            break;
        case 's':
            printf("Benchmarking SIMD convolution...\n");
            benchmark((void (*)(float**, float**, float**, int, int))convolve_simd,
                      matrix, kernel, output, k, out_size);
            break;
        case 'a':
            printf("All convolutions will be benchmarked.\n");
            printf("Benchmarking row-major convolution...\n");
            benchmark((void (*)(float**, float**, float**, int, int))convolve_row_major,
                      matrix, kernel, output, k, out_size);
            printf("Benchmarking column-major convolution...\n");
            benchmark((void (*)(float**, float**, float**, int, int))convolve_col_major,
                      matrix, kernel, output, k, out_size);
            printf("Benchmarking SIMD convolution...\n");
            benchmark((void (*)(float**, float**, float**, int, int))convolve_simd,
                      matrix, kernel, output, k, out_size);
            break;
        default:
                fprintf(stderr, "Usage: ./prog [-r|-c|-s|-a] <size> <kernel> \n -r: Row-major convolution \n -c: Column-major convolution \n -s: SIMD convolution \n -a: All convolutions \n");
                fprintf(stderr, "Example: ./prog -r 1024 3\n");
                free_matrix(matrix, n);
                free_matrix(kernel, k);
                free_matrix(output, out_size);
                return 1;
    }

    // Free allocated memory
    printf("Freeing allocated memory...\n");
    free_matrix(matrix, n);
    free_matrix(kernel, k);
    free_matrix(output, out_size);
    return 0;
}


