#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Prefix Sum secuencial inclusivo
void scan_secuencial(const int *a, int *r, int n) {
    int suma = 0;
    for (int i = 0; i < n; i++) {
        suma += a[i];
        r[i] = suma;
    }
}

// Blelloch paralelo: resultado exclusivo
// n debe ser potencia de dos
void blelloch_scan_parallel(int *arr, int n) {

    // 1. Fase Up-sweep
    for (int stride = 1; stride < n; stride *= 2) {
        #pragma omp parallel for
        for (int i = stride - 1; i < n; i += stride * 2) {
            arr[i + stride] += arr[i];
        }
    }

    // Preparacion del resultado exclusivo
    arr[n - 1] = 0;

    // 2. Fase Down-sweep
    for (int stride = n / 2; stride > 0; stride /= 2) {
        #pragma omp parallel for
        for (int i = stride - 1; i < n; i += stride * 2) {
            int t = arr[i];
            arr[i] = arr[i + stride];
            arr[i + stride] += t;
        }
    }
}

// Imprimir elementos de un arreglo
void imprimir(const char *etiqueta, const int *a, int n) {
    printf("%s", etiqueta);
    for (int i = 0; i < n; i++) {
        printf("%d%s", a[i], i == n-1 ? "\n" : " ");
    }
}

int main(void) {

    // DEMOSTRACION CON 8 ELEMENTOS
    int original[8] = {1,2,3,4,5,6,7,8};
    int scan[8];

    memcpy(scan, original, sizeof original);

    omp_set_num_threads(4);
    blelloch_scan_parallel(scan, 8);

    imprimir("Original:  ", original, 8);
    imprimir("Exclusivo: ", scan, 8);

    // Conversion a Prefix Sum inclusivo
    for (int i = 0; i < 8; i++) {
        scan[i] += original[i];
    }

    imprimir("Inclusivo: ", scan, 8);

    // COMPARACION CON ARREGLO GRANDE
    const int n = 1 << 20;
    const int repeticiones = 5;

    int *datos = malloc((size_t)n * sizeof(int));
    int *secuencial = malloc((size_t)n * sizeof(int));
    int *paralelo = malloc((size_t)n * sizeof(int));

    if (!datos || !secuencial || !paralelo) {
        fprintf(stderr, "Error de memoria.\n");
        free(datos);
        free(secuencial);
        free(paralelo);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        datos[i] = i % 8 + 1;
    }

    // Medir ejecucion secuencial
    double tiempo_sec = 0.0;

    for (int r = 0; r < repeticiones; r++) {
        double inicio = omp_get_wtime();

        scan_secuencial(datos, secuencial, n);

        tiempo_sec += omp_get_wtime() - inicio;
    }

    tiempo_sec /= repeticiones;

    printf("\nArreglo grande (%d elementos)\n", n);
    printf("Secuencial: %.6f s\n", tiempo_sec);

    // Medir ejecucion paralela
    int hilos[] = {1,2,4,8};

    for (int k = 0; k < 4; k++) {
        omp_set_num_threads(hilos[k]);

        double tiempo_par = 0.0;

        for (int r = 0; r < repeticiones; r++) {
            double inicio = omp_get_wtime();

            memcpy(paralelo, datos, (size_t)n * sizeof(int));
            blelloch_scan_parallel(paralelo, n);

            // Conversion a inclusivo
            #pragma omp parallel for
            for (int i = 0; i < n; i++) {
                paralelo[i] += datos[i];
            }

            tiempo_par += omp_get_wtime() - inicio;

            // Verificar resultados
            if (memcmp(paralelo, secuencial,
                       (size_t)n * sizeof(int)) != 0) {
                printf("Error: resultados diferentes\n");
                free(datos);
                free(secuencial);
                free(paralelo);
                return 1;
            }
        }

        tiempo_par /= repeticiones;

        double speedup = tiempo_sec / tiempo_par;
        double eficiencia = 100.0 * speedup / hilos[k];

        printf("%d hilos: %.6f s | Speedup: %.3f"
               " | Eficiencia: %.1f%% | OK\n",
               hilos[k], tiempo_par,
               speedup, eficiencia);
    }

    free(datos);
    free(secuencial);
    free(paralelo);

    return 0;
}
