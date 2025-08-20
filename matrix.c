#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <immintrin.h>

float** nxn(int n) {
    float** matrice = malloc(n * sizeof(float*));
    if (!matrice) {
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        matrice[i] = malloc(n * sizeof(float));
        if (!matrice[i]) {
            for (int j = 0; j < i; j++) {
                free(matrice[j]);
            }
            free(matrice);
            return NULL;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrice[i][j] = rand() / (float)RAND_MAX;
        }
    }

    return matrice;
}

float** kxk(int k) {
    float** matrice = malloc(k * sizeof(float*));
    if (!matrice) {
        return NULL;
    }

    float value = 1.0 / (k * k);

    for (int i = 0; i < k; i++) {
        matrice[i] = malloc(k * sizeof(float));
        if (!matrice[i]) {
            for (int j = 0; j < i; j++) {
                free(matrice[j]);
            }
            free(matrice);
            return NULL;
        }
    }

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            matrice[i][j] = value;
        }
    }

    return matrice;
}

void convolve_row_major(float** matrix, float** kernel, float** output,
                        int k, int out_size){
    // Convolution (row-major)
    for (int i = 0; i < out_size; i++) {
        for (int j = 0; j < out_size; j++) {
            float sum = 0.0f;

            // Parcours du kernel
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    sum += matrix[i + ki][j + kj] * kernel[ki][kj];
                }
            }
            output[i][j] = sum;
        }
    }

}


void convolve_col_major(float** matrix, float** kernel, float** output,
                        int k, int out_size){

    // Convolution (col-major)
    for (int j = 0; j < out_size; j++) {
        for (int i = 0; i < out_size; i++) {
            float sum = 0.0f;
            // Parcours du kernel
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    sum += matrix[i + ki][j + kj] * kernel[ki][kj];
                }
            }
            output[i][j] = sum;
        }
    }
}


void convolve_simd(float** matrix, float** kernel, float** output,
                   int k, int out_size){
    for (int i = 0; i < out_size; i++) {
        int j = 0;

        // Traiter 8 colonnes de sortie en parallèle
        for (; j <= out_size - 8; j += 8) {
            __m256 vec_sum = _mm256_setzero_ps();

            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    __m256 mat_vals = _mm256_loadu_ps(&matrix[i + ki][j + kj]);
                    __m256 ker_val = _mm256_set1_ps(kernel[ki][kj]);
                    vec_sum = _mm256_fmadd_ps(mat_vals, ker_val, vec_sum);
                }
            }

            _mm256_storeu_ps(&output[i][j], vec_sum);
        }

        // Colonnes restantes
        for (; j < out_size; j++) {
            float sum = 0.0f;
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj++) {
                    sum += matrix[i + ki][j + kj] * kernel[ki][kj];
                }
            }
            output[i][j] = sum;
        }
    }
}


/*  
    // Convolution (SIMD)
    for (int i = 0; i < out_size; i++) {
        for (int j = 0; j < out_size; j++) {
            __m256 sum = _mm256_setzero_ps(); // Initialisation du registre SIMD
            
            // Parcours du kernel
            for (int ki = 0; ki < k; ki++) {
                for (int kj = 0; kj < k; kj += 8) {
                    // Chargement des valeurs du kernel et de la matrice dans des registres SIMD
                    __m256 kernel_values = _mm256_loadu_ps(&kernel[ki][kj]);
                    __m256 matrix_values = _mm256_loadu_ps(&matrix[i + ki][j + kj]);
                    
                    // Multiplication et accumulation
                    sum = _mm256_add_ps(sum, _mm256_mul_ps(matrix_values, kernel_values));
                }
            }
            // Réduction des valeurs SIMD en une seule valeur
            float result[8];
            _mm256_storeu_ps(result, sum);
            float final_sum = 0.0f;
            for (int r = 0; r < 8; r++) {
                final_sum += result[r];
            }
            output[i][j] = final_sum;
        }
    }
    return output;
*/
