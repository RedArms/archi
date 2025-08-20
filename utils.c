#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "matrix.h"

void print_matrix(float *matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%f ", matrix[i * n + j]);
        }
        printf("\n");
    }
}

void free_matrix(float *matrix) {
    free(matrix);
}

void clear_matrix(float *matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = 0.0f;
        }
    }
}

void benchmark(void (*fun)(float*, float*, float*, int, int),
               float* matrix, float* kernel, float* output,
               int k, int out_size) {
    const int repeats = 5;
    double times_ms[repeats];
    double total_ms = 0.0;

    for (int r = 0; r < repeats; r++) {
        struct timespec start, end;
        clear_matrix(output, out_size);
        clock_gettime(CLOCK_MONOTONIC, &start);
        fun(matrix, kernel, output, k, out_size);
        clock_gettime(CLOCK_MONOTONIC, &end);
        long seconds = end.tv_sec - start.tv_sec;
        long nanoseconds = end.tv_nsec - start.tv_nsec;
        if (nanoseconds < 0) {
            seconds--;
            nanoseconds += 1000000000L;
        }
        times_ms[r] = seconds * 1000.0 + nanoseconds / 1.0e6;
        total_ms += times_ms[r];
    }

    double average = total_ms / repeats;
    double variance = 0.0;
    for (int r = 0; r < repeats; r++) {
        double diff = times_ms[r] - average;
        variance += diff * diff;
    }
    variance /= repeats;
    double stddev = sqrt(variance);

    printf("Average runtime over %d runs: %.3f ms\n", repeats, average);
    printf("Standard deviation: %.3f ms\n", stddev);

    double pixels = (double)out_size * out_size;
    double throughput = pixels / (average / 1000.0);
    printf("Throughput: %.2f pixels/s\n", throughput);
}
