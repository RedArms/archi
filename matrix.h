#ifndef MATRICE_H
#define MATRICE_H

float** nxn(int n);
float** kxk(int k);

void convolve_row_major(float** matrix, float** kernel, float** output,
                        int k, int out_size);
void convolve_col_major(float** matrix, float** kernel, float** output,
                        int k, int out_size);
void convolve_simd(float** matrix, float** kernel, float** output,
                   int k, int out_size);
 
#endif
