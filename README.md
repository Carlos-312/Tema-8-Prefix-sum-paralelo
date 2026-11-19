# Tema-8-Prefix-sum-paralelo

---

# Integrantes
- [323255343] - Gil Isaias Kevin
- [número de cuenta] - Nombre
- [número de cuenta] - Nombre
- [número de cuenta] - Nombre

---

# Contexto y teoría

**Concepto:** El tema central es la resolución computacional de la suma de prefijos de forma paralela mediante el algoritmo de Blelloch (también conocido como *scan*).

**Objetivo:** El propósito de la exposición es mostrar cómo calcular sumas acumuladas en paralelo manteniendo una eficiencia de trabajo de `O(n)` y reduciendo drásticamente la profundidad computacional a `O(log n)`.

**¿Qué es Prefix Sum?** 
Prefix Sum, también conocido como suma acumulada, es una operación que consiste en sumar progresivamente los elementos de un arreglo. Se utiliza en diferentes áreas de la computación, como el procesamiento de datos, imágenes y algoritmos paralelos.

**Relación con la Ley de Brent, Trabajo y Profundidad:**
La Ley de Brent establece que un algoritmo con un trabajo total `T1` y una profundidad `T_inf` puede ejecutarse en `p` procesadores con un tiempo limitado por `Tp <= (T1 / p) + T_inf`. 
Aunque el algoritmo de suma secuencial tradicional realiza un trabajo lineal (`O(n)`), su naturaleza es estrictamente dependiente (el paso actual depende del anterior), lo que deja su profundidad también en `O(n)`. Otros algoritmos paralelos básicos (como el de Hillis-Steele) bajan la profundidad a `O(log n)` pero desperdician ciclos aumentando el trabajo a `O(n log n)`. 
El algoritmo de Blelloch brilla porque es *work-efficient* (óptimo en trabajo): reorganiza las operaciones en forma de árbol, conservando el trabajo asintótico en `O(n)` y alcanzando una profundidad logarítmica de `O(log n)`.

**Independencia de iteraciones para `parallel for`:**
Para aplicar paralelismo de datos iterativo con `#pragma omp parallel for`, las iteraciones deben ser independientes. En el algoritmo de Blelloch, esto se logra porque en un nivel determinado del árbol, cada hilo opera con celdas de memoria completamente separadas por un "salto" (*stride*). Al no haber cruces ni dos hilos escribiendo en la misma celda a la vez, se evitan las condiciones de carrera sin usar bloqueos.

---

# Demostraciones

**Ejemplo de Prefix Sum:**
Consideremos el siguiente arreglo:

``
A = [1, 2, 3, 4, 5, 6, 7, 8]
``

Su suma acumulada es:

``
R = [1, 3, 6, 10, 15, 21, 28, 36]
``

Cada posición contiene la suma de los elementos anteriores, incluyendo el elemento actual.

Por ejemplo:

``
Posición 0: 1

Posición 1: 1 + 2 = 3

Posición 2: 1 + 2 + 3 = 6

Posición 3: 1 + 2 + 3 + 4 = 10
``


El algoritmo logra la exclusividad (*exclusive scan*) dividiendo el proceso en dos fases estructurales: **up-sweep** y **down-sweep**. 

### Fases sobre un arreglo pequeño (8 elementos)
Analicemos las fases sobre este arreglo inicial: `[3, 1, 7, 0, 4, 1, 6, 3]`

**1. Fase Up-sweep (Reducción)**:
Avanza desde las hojas hacia la raíz sumando bloques de tamaño creciente.
*   **Paso 1 (stride 1):** Suma por pares. `[3, (3+1)=4, 7, (7+0)=7, 4, (4+1)=5, 6, (6+3)=9]`  
    Queda: `[3, 4, 7, 7, 4, 5, 6, 9]`
*   **Paso 2 (stride 2):** Suma de a cuatro.  
    Queda: `[3, 4, 7, 11, 4, 5, 6, 14]`
*   **Paso 3 (stride 4):** Suma final en la raíz.  
    Queda: `[3, 4, 7, 11, 4, 5, 6, 25]` (25 es la suma total).

**2. Fase Down-sweep (Distribución)**:
El algoritmo exclusivo fuerza el nodo raíz a `0` y propaga los valores acumulados de vuelta a las hojas.
*   **Paso Inicial:** Raíz a cero. `[3, 4, 7, 11, 4, 5, 6, 0]`
*   **Paso 1 (stride 4):** Intercambia el 0 con el nodo hijo izquierdo y suma.  
    Queda: `[3, 4, 7, 0, 4, 5, 6, 11]` 
*   **Paso 2 (stride 2):**  
    Queda: `[3, 0, 7, 4, 4, 11, 6, 16]`
*   **Paso 3 (stride 1):**  
    Resultado final Exclusivo: `[0, 3, 4, 11, 11, 15, 16, 22]`


# Demo en C con OpenMP
A continuación, una implementación del *Prefix Sum* en C implementando la división del trabajo en cada nivel del árbol:

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
