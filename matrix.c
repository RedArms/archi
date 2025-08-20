#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <immintrin.h>

float** nxn(int n) {

    float** matrice = malloc(n * sizeof(float*));

    for (int i = 0; i < n; i++) {
        matrice[i] = malloc(n * sizeof(float));
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
    float value = 1.0 / (k * k);

    for (int i = 0; i < k; i++) {
        matrice[i] = malloc(k * sizeof(float));
    }

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            matrice[i][j] = value;
        }
    }
    return matrice;


}

float** convolve_row_major(float** matrix, float** kernel,float** output,
                           int n, int k,int out_size){
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

    return output;
    }


float** convolve_col_major(float** matrix, float** kernel,float** output,
                           int n, int k,int out_size){

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
    return output;
    }


float** convolve_simd(float** matrix, float** kernel,float** output,
                      int n, int k,int out_size){

    for (int i = 0; i < out_size; i++) {
        for (int j = 0; j < out_size; j++) {

            float sum = 0.0f;

            for (int ki = 0; ki < k; ki++) {

                int kj = 0;

                // SIMD - 8 float à la fois
                for (; kj <= k - 8; kj += 8) {
                    // Charger 8 floats de la matrice (ligne non contiguë)
                    __m256 mat_vals = _mm256_set_ps(
                        matrix[i + ki][j + kj + 7],
                        matrix[i + ki][j + kj + 6],
                        matrix[i + ki][j + kj + 5],
                        matrix[i + ki][j + kj + 4],
                        matrix[i + ki][j + kj + 3],
                        matrix[i + ki][j + kj + 2],
                        matrix[i + ki][j + kj + 1],
                        matrix[i + ki][j + kj + 0]
                    );

                    __m256 ker_vals = _mm256_set_ps(
                        kernel[ki][kj + 7],
                        kernel[ki][kj + 6],
                        kernel[ki][kj + 5],
                        kernel[ki][kj + 4],
                        kernel[ki][kj + 3],
                        kernel[ki][kj + 2],
                        kernel[ki][kj + 1],
                        kernel[ki][kj + 0]
                    );

                    // Multiplier
                    __m256 prod = _mm256_mul_ps(mat_vals, ker_vals);

                    // Réduction horizontale manuelle
                    __m128 low = _mm256_castps256_ps128(prod);
                    __m128 high = _mm256_extractf128_ps(prod, 1);
                    __m128 sum128 = _mm_add_ps(low, high);
                    sum128 = _mm_hadd_ps(sum128, sum128);
                    sum128 = _mm_hadd_ps(sum128, sum128);
                    sum += _mm_cvtss_f32(sum128);
                }

                // Reste des scalaires
                for (; kj < k; kj++) {
                    sum += matrix[i + ki][j + kj] * kernel[ki][kj];
                }
            }

            output[i][j] = sum;
        }
    }

    return output;
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
