# Tema-8-Prefix-sum-paralelo

---

# Integrantes
- [número de cuenta] - Nombre
- [número de cuenta] - Nombre
- [número de cuenta] - Nombre
- [número de cuenta] - Nombre

---

# Contexto y teoría

**Concepto:** El tema central es la resolución computacional de la suma de prefijos de forma paralela mediante el algoritmo de Blelloch (también conocido como *scan*)[span_2](start_span)[span_2](end_span).
**Objetivo:** El propósito de la exposición es mostrar cómo calcular sumas acumuladas en paralelo manteniendo una eficiencia de trabajo de $O(n)$ y reduciendo drásticamente la profundidad computacional a $O(\log n)$[span_3](start_span)[span_3](end_span).

**Relación con la Ley de Brent, Trabajo y Profundidad:**
La Ley de Brent establece que un algoritmo con un trabajo total $T_1$ y una profundidad $T_\infty$ puede ejecutarse en $p$ procesadores con un tiempo limitado por $T_p \le (T_1 / p) + T_\infty$[span_4](start_span)[span_4](end_span). 
Aunque el algoritmo de suma secuencial tradicional realiza un trabajo lineal ($O(n)$), su naturaleza es estrictamente dependiente (el paso actual depende del anterior), lo que deja su profundidad también en $O(n)$[span_5](start_span)[span_5](end_span). Otros algoritmos paralelos básicos (como el de Hillis-Steele) bajan la profundidad a $O(\log n)$ pero desperdician ciclos aumentando el trabajo a $O(n \log n)$. 
El algoritmo de Blelloch brilla porque es *work-efficient* (óptimo en trabajo): reorganiza las operaciones en forma de árbol, conservando el trabajo asintótico en $O(n)$ y alcanzando una profundidad logarítmica de $O(\log n)$[span_6](start_span)[span_6](end_span).

**Independencia de iteraciones para `parallel for`:**
Para aplicar paralelismo de datos iterativo con `#pragma omp parallel for`, las iteraciones deben ser independientes[span_7](start_span)[span_7](end_span). En el algoritmo de Blelloch, esto se logra porque en un nivel determinado del árbol, cada hilo opera con celdas de memoria completamente separadas por un "salto" (*stride*). Al no haber cruces ni dos hilos escribiendo en la misma celda a la vez, se evitan las condiciones de carrera sin usar bloqueos[span_8](start_span)[span_8](end_span).

---

# Demostraciones

El algoritmo logra la exclusividad (*exclusive scan*) dividiendo el proceso en dos fases estructurales: **up-sweep** y **down-sweep**[span_9](start_span)[span_9](end_span). 

### Fases sobre un arreglo pequeño (8 elementos)
Analicemos las fases sobre este arreglo inicial[span_10](start_span)[span_10](end_span): `[3, 1, 7, 0, 4, 1, 6, 3]`

**1. Fase Up-sweep (Reducción)**[span_11](start_span)[span_11](end_span):
Avanza desde las hojas hacia la raíz sumando bloques de tamaño creciente.
*   **Paso 1 (stride 1):** Suma por pares. `[3, (3+1)=4, 7, (7+0)=7, 4, (4+1)=5, 6, (6+3)=9]`  
    Queda: `[3, 4, 7, 7, 4, 5, 6, 9]`
*   **Paso 2 (stride 2):** Suma de a cuatro.  
    Queda: `[3, 4, 7, 11, 4, 5, 6, 14]`
*   **Paso 3 (stride 4):** Suma final en la raíz.  
    Queda: `[3, 4, 7, 11, 4, 5, 6, 25]` (25 es la suma total).

**2. Fase Down-sweep (Distribución)**[span_12](start_span)[span_12](end_span):
El algoritmo exclusivo fuerza el nodo raíz a `0` y propaga los valores acumulados de vuelta a las hojas.
*   **Paso Inicial:** Raíz a cero. `[3, 4, 7, 11, 4, 5, 6, 0]`
*   **Paso 1 (stride 4):** Intercambia el 0 con el nodo hijo izquierdo y suma.  
    Queda: `[3, 4, 7, 0, 4, 5, 6, 11]` 
*   **Paso 2 (stride 2):**  
    Queda: `[3, 0, 7, 4, 4, 11, 6, 16]`
*   **Paso 3 (stride 1):**  
    Resultado final Exclusivo: `[0, 3, 4, 11, 11, 15, 16, 22]`

### Demo en C con OpenMP
A continuación, una implementación del *Prefix Sum* en C implementando la división del trabajo en cada nivel del árbol[span_13](start_span)[span_13](end_span):

```c
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void blelloch_scan_parallel(int *arr, int n) {
    // 1. Fase Up-sweep
    for (int stride = 1; stride < n; stride *= 2) {
        #pragma omp parallel for
        for (int i = stride - 1; i < n; i += stride * 2) {
            arr[i + stride] += arr[i];
        }
    }
    
    // Preparación para el arreglo exclusivo
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
