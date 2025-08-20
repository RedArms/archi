#ifndef UTILS_H
#define UTILS_H

void print_matrix(float **matrix, int n);
void free_matrix(float **matrix, int n);
void clear_matrix(float** matrix, int n);
void benchmark(float** (*fun) (float**, float**,float**, int, int),
               float** matrix, float** kernel,float** output,
               int k, int out_size);
#endif
