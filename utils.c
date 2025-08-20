#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "matrix.h"

void print_matrix(float **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}

void free_matrix(float** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void clear_matrix(float** matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = 0.0f;
        }
    }
}

void benchmark(float** (*fun)(float**, float**,float**, int, int, int),
               float** matrix, float** kernel,float** output,
               int n, int k, int out_size) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    output = fun(matrix, kernel, output, n, k, out_size);
    clock_gettime(CLOCK_MONOTONIC, &end);
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    double elapsed_ms = seconds * 1000.0 + nanoseconds / 1.0e6;
    //print_matrix(output, out_size);
    clear_matrix(output, out_size);
    printf("Benchmark completed in %.3f ms.\n", elapsed_ms);
}
