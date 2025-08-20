#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <immintrin.h>

// Allocate an n x n matrix with contiguous memory
float* nxn(int n) {
    float* matrix = malloc(n * n * sizeof(float));
    if (!matrix) {
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i * n + j] = rand() / (float)RAND_MAX;
        }
    }
    return matrix;
}

// Allocate a k x k kernel with contiguous memory and fill with constant value
float* kxk(int k) {
    float* matrix = malloc(k * k * sizeof(float));
    if (!matrix) {
        return NULL;
    }
    float value = 1.0f / (k * k);
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            matrix[i * k + j] = value;
        }
    }
    return matrix;
}

// Row-major convolution
void convolve_row_major(float* matrix, float* kernel, float* output,
                        int k, int out_size) {
    int n = out_size + k - 1;
    for (int i = 0; i < out_size; i++) {
        for (int j = 0; j < out_size; j++) {
            float sum = 0.0f;
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    sum += matrix[(i + ki) * n + (j + kj)] *
                           kernel[ki * k + kj];
                }
            }
            output[i * out_size + j] = sum;
        }
    }
}

// Column-major convolution
void convolve_col_major(float* matrix, float* kernel, float* output,
                        int k, int out_size) {
    int n = out_size + k - 1;
    for (int j = 0; j < out_size; j++) {
        for (int i = 0; i < out_size; i++) {
            float sum = 0.0f;
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    sum += matrix[(i + ki) * n + (j + kj)] *
                           kernel[ki * k + kj];
                }
            }
            output[i * out_size + j] = sum;
        }
    }
}

// SIMD-optimized convolution
void convolve_simd(float* matrix, float* kernel, float* output,
                   int k, int out_size) {
    int n = out_size + k - 1;
    for (int i = 0; i < out_size; i++) {
        int j = 0;
        for (; j <= out_size - 8; j += 8) {
            __m256 vec_sum = _mm256_setzero_ps();
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    __m256 mat_vals = _mm256_loadu_ps(
                        &matrix[(i + ki) * n + j + kj]);
                    __m256 ker_val = _mm256_set1_ps(kernel[ki * k + kj]);
                    vec_sum = _mm256_fmadd_ps(mat_vals, ker_val, vec_sum);
                }
            }
            _mm256_storeu_ps(&output[i * out_size + j], vec_sum);
        }
        for (; j < out_size; j++) {
            float sum = 0.0f;
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    sum += matrix[(i + ki) * n + (j + kj)] *
                           kernel[ki * k + kj];
                }
            }
            output[i * out_size + j] = sum;
        }
    }
}
