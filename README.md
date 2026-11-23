# Tema-8-Prefix-sum-paralelo

---

# Integrantes
- [323255343] - Gil Isaias Kevin
- [323273613] - Sánchez Ledesma Paulina
- [323111047] - Montes Alcántara Diego
- [323080549] - Flores Reyes Carlos Roberto

---

## ¿Qué es Prefix Sum?

Prefix Sum, también conocido como suma de prefijos o suma acumulada, es una operación que permite calcular progresivamente las sumas de los elementos de un arreglo. Su objetivo es obtener, para cada posición, la suma de los elementos anteriores o hasta la posición actual.

Se utiliza en diferentes áreas de la computación, como el procesamiento de datos, imágenes y algoritmos paralelos. Además, no se limita únicamente a la suma, ya que también puede aplicarse a operaciones asociativas, como la multiplicación, el máximo o el mínimo. Esta propiedad permite reorganizar los cálculos sin modificar el resultado.

## Prefix Sum Paralelo

Tradicionalmente, las sumas acumuladas se calculan de manera secuencial, donde cada operación depende del resultado anterior. Esta dependencia dificulta que los cálculos se realicen simultáneamente.

Para superar esta limitación, Prefix Sum Paralelo reorganiza las operaciones para distribuirlas entre varios hilos o procesadores. En lugar de calcular las sumas consecutivamente, utiliza resultados parciales que pueden obtenerse al mismo tiempo y combinarse posteriormente.Esto es posible gracias a la propiedad asociativa de la suma, que permite agrupar los elementos de distintas maneras sin alterar el resultado.

Entre los algoritmos utilizados se encuentran Hillis-Steele y Blelloch. Aunque ambos reducen la profundidad computacional a \(O(\log n)\), Blelloch destaca porque mantiene un trabajo total de \(O(n)\), logrando una mayor eficiencia teórica.

## Algoritmo de Blelloch

El algoritmo de Blelloch utiliza una estructura de árbol binario para organizar las operaciones en diferentes niveles, permitiendo ejecutar simultáneamente los cálculos independientes.

Su funcionamiento se divide en dos fases:
- Up-sweep (reducción ascendente): consiste en combinar los elementos del arreglo para obtener sumas parciales de grupos cada vez más grandes, hasta calcular la suma total. En cada nivel, las operaciones de grupos diferentes pueden ejecutarse simultáneamente.
- Down-sweep (distribución descendente): comienza reemplazando por cero el valor de la raíz y distribuye las sumas parciales a través del árbol. De esta manera, cada posición obtiene la suma de los elementos anteriores, generando el Prefix Sum exclusivo.

## Independencia de operaciones y OpenMP

Una característica fundamental de Blelloch es que las operaciones de un mismo nivel son independientes, porque trabajan sobre posiciones diferentes del arreglo.Para organizar estos cálculos se utiliza un desplazamiento llamado stride o salto, que determina qué elementos deben combinarse en cada nivel. Durante Up-sweep, este salto aumenta progresivamente, mientras que en Down-sweep disminuye.

Esta característica permite implementar el algoritmo mediante OpenMP, utilizando la directiva #pragma omp parallel for para distribuir las operaciones entre diferentes hilos.Al trabajar sobre posiciones independientes, se evitan las condiciones de carrera dentro de cada nivel. Sin embargo, los niveles consecutivos sí presentan dependencias, por lo que es necesario sincronizar los hilos antes de continuar con el siguiente.

## Relación con la Ley de Brent, Trabajo y Profundidad

La Ley de Brent establece que un algoritmo con un trabajo total `T1` y una profundidad `T_inf` puede ejecutarse en `p` procesadores con un tiempo limitado por `Tp <= (T1 / p) + T_inf`. 
Aunque el algoritmo de suma secuencial tradicional realiza un trabajo lineal (`O(n)`), su naturaleza es estrictamente dependiente (el paso actual depende del anterior), lo que deja su profundidad también en `O(n)`. 

Otros algoritmos paralelos básicos (como el de Hillis-Steele) bajan la profundidad a `O(log n)` pero desperdician ciclos aumentando el trabajo a `O(n log n)`. 
El algoritmo de Blelloch brilla porque es *work-efficient* (óptimo en trabajo): reorganiza las operaciones en forma de árbol, conservando el trabajo asintótico en `O(n)` y alcanzando una profundidad logarítmica de `O(log n)`.

---

# Demostraciones

## Ejemplo de Prefix Sum:

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

```
Posición 0: 1

Posición 1: 1 + 2 = 3

Posición 2: 1 + 2 + 3 = 6

Posición 3: 1 + 2 + 3 + 4 = 10
```

## Fases sobre un arreglo pequeño (8 elementos)

El algoritmo de Blelloch permite realizar Prefix Sum utilizando una estructura de árbol. Su funcionamiento se divide en dos fases principales:

### Up-sweep (fase de subida)

En esta fase se calculan sumas parciales hasta obtener la suma total del arreglo.

Utilizando el arreglo:

``
A = [1, 2, 3, 4, 5, 6, 7, 8]
``

Primer nivel: sumas por parejas

``
1 + 2 = 3
3 + 4 = 7
5 + 6 = 11
7 + 8 = 15
``

Segundo nivel:

``
3 + 7 = 10
11 + 15 = 26
``

Tercer nivel:

``
10 + 26 = 36
``

Al finalizar esta fase obtenemos la suma total del arreglo: 36.

### Down-sweep (fase de bajada)

La fase Down-sweep es la segunda etapa del algoritmo de Blelloch. Su objetivo es distribuir las sumas parciales obtenidas en Up-sweep para calcular los prefijos de cada posición.

Para comenzar, se reemplaza la suma total de la raíz por un cero, ya que inicialmente se calcula una suma exclusiva (sin incluir el elemento actual).

Utilizaremos el arreglo:

``
A = [1, 2, 3, 4, 5, 6, 7, 8]
``

¿Cómo funciona?

El algoritmo recorre el árbol de arriba hacia abajo siguiendo dos reglas:
- El hijo izquierdo recibe el valor de su padre.
- El hijo derecho recibe el valor del padre más la suma original del hijo izquierdo.

**Paso 1. Primer nivel**

Las sumas originales de las ramas eran 10 y 26. Comenzamos con cero:
```
Izquierda = 0

Derecha = 0 + 10 = 10

Resultado: [0, 10]
```

**Paso 2. Segundo nivel**

Distribuimos los valores hacia los siguientes nodos:
```
Izquierda: 0 y 0 + 3 = 3

Derecha: 10 y 10 + 11 = 21

Resultado: [0, 3, 10, 21]
```

**Paso 3. Tercer nivel**

Repetimos el procedimiento hasta llegar a las posiciones originales:
```
Pareja -------- Izquierda -------- Derecha

1 y 2 ------------ 0 --------------- 0 + 1 = 1

3 y 4 ------------ 3 --------------- 3 + 3 = 6

5 y 6 ------------ 10 -------------- 10 + 5 = 15

7 y 8 ------------ 21 -------------- 21 + 7 = 28
```

Obtenemos el resultado exclusivo:
``
[0, 1, 3, 6, 10, 15, 21, 28]
``

Resultado inclusivo

Para obtener la suma acumulada normal, sumamos cada elemento original con su resultado exclusivo:
``
Resultado[i] = A[i] + ResultadoExclusivo[i]
``

Resultado final:
``
[1, 3, 6, 10, 15, 21, 28, 36]
``

De esta manera se comprueba que el algoritmo de Blelloch permite obtener las mismas sumas acumuladas que el procedimiento secuencial, pero mediante operaciones organizadas en distintos niveles.

---

# ¿Por qué las iteraciones de cada nivel son independientes?

Una de las principales ventajas del algoritmo de Blelloch es que permite ejecutar varias operaciones simultáneamente (paralelismo).

Por ejemplo, en el primer nivel del arreglo tenemos:

```
1 + 2 = 3

3 + 4 = 7

5 + 6 = 11

7 + 8 = 15
```

Estas cuatro operaciones son independientes porque ninguna necesita el resultado de otra. Por esta razón, podemos distribuirlas entre diferentes hilos de ejecución.

    Hilo 0        Hilo 1         Hilo 2         Hilo 3
    1 + 2         3 + 4           5 + 6          7 + 8
      ↓             ↓               ↓              ↓
      3             7               11             15

Sin embargo, debemos esperar a que termine un nivel antes de comenzar el siguiente, ya que los resultados de un nivel se utilizan en el siguiente. Algo importante es que las operaciones de un mismo nivel pueden ejecutarse en paralelo, mientras que entre niveles consecutivos si deben respetarse las dependencias, utilizan los resultados calculados anteriormente. Por esta razón, es necesario sincronizar los hilos antes de comenzar el siguiente nivel.

Esta característica es la que permite utilizar la directiva `#pragma omp parallel for` para repartir las iteraciones entre varios hilos.

---

# Trabajo y profundidad

Para analizar el rendimiento de un algoritmo paralelo se utilizan dos conceptos:
- Trabajo: representa la cantidad total de operaciones que realiza el algoritmo.
- Profundidad: representa la cantidad de niveles de operaciones dependientes que deben ejecutarse.

**Comparación de algoritmos:**
```
Característica --------- Scan secuencial --------- Blelloch paralelo

Trabajo --------------------- O(n) ---------------------- O(n)

Profundidad ----------------- O(n) -------------------- O(log n)

Ejecución ---------- Una operación tras otra -- Operaciones simultáneas por nivel
```

El algoritmo secuencial necesita recorrer el arreglo de manera progresiva, mientras que Blelloch organiza las operaciones en forma de árbol.

Para un arreglo de 8 elementos:
```
Up-sweep:   3 niveles

Down-sweep: 3 niveles

Profundidad total: 6 niveles
```

En general, para n elementos (cuando n es potencia de 2), el algoritmo tiene:
```
Trabajo:     O(n)

Profundidad: O(log n)
```

Esto permite reducir la profundidad respecto al método secuencial.

---

## Demo en C con OpenMP
Para demostrar el funcionamiento de Prefix Sum Paralelo, se implementó el algoritmo de Blelloch en lenguaje C utilizando OpenMP, con el objetivo de distribuir las operaciones entre diferentes hilos en cada nivel del árbol.

El programa desarrolla las fases Up-sweep y Down-sweep mediante la directiva #pragma omp parallel for, que permite ejecutar simultáneamente las operaciones independientes. Además, se incluye una implementación secuencial para comprobar los resultados y comparar los tiempos de ejecución utilizando un arreglo de mayor tamaño.

```c
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
```

## Resultados de la demostración

```
Original:  1 2 3 4 5 6 7 8
Exclusivo: 0 1 3 6 10 15 21 28
Inclusivo: 1 3 6 10 15 21 28 36
```

Los resultados obtenidos permiten comprobar que el algoritmo de Blelloch calcula correctamente las sumas acumuladas. El resultado exclusivo coincide con el procedimiento de Down-sweep desarrollado manualmente, mientras que la versión inclusiva corresponde a la suma acumulada convencional.

## Tabla comparativa de tiempos 

Para evaluar el rendimiento de Prefix Sum Paralelo, se realizaron pruebas utilizando un arreglo de 1,048,576 elementos, comparando el algoritmo secuencial con la implementación de Blelloch en OpenMP.

La versión paralela se ejecutó utilizando 1, 2, 4 y 8 hilos, con el objetivo de observar cómo cambia el tiempo al distribuir el trabajo entre diferentes cantidades de hilos.

```
     Implementación        |  Hilos  |   Tiempo promedio (s)  |   Speedup   |   Eficiencia 
 Prefix Sum secuencial     |    1    |        0.001247	      |    1.000    |      --
   Blelloch paralelo       |    1    |        0.002922	      |    0.427    |     42.7%
   Blelloch paralelo       |    2    |        0.003308        |    0.377    |     18.8%
   Blelloch paralelo       |    4    |        0.004543        |	   0.274    |      6.9%
   Blelloch paralelo       |    8    |        0.006903	      |    0.181    |      2.3#
```
La versión secuencial tiene un speedup de 1 y una eficiencia teórica de referencia del 100%, ya que se compara consigo misma utilizando un solo hilo, sin embargo, se coloca un guion en la eficiencia, ya que no se evalúa el paralelismo.

## Análisis gráfico del rendimiento
<img width="752" height="452" alt="image" src="https://github.com/user-attachments/assets/028ab551-3a54-4631-b722-bc60f15ea938" />

Los resultados muestran que la implementación secuencial obtuvo el menor tiempo de ejecución (0.001247 s), mientras que Blelloch fue más lento conforme aumentó la cantidad de hilos. Además, el speedup se mantuvo por debajo de 1 y la eficiencia disminuyó progresivamente.

Este comportamiento puede deberse a los costos de sincronización y administración de los hilos. Aunque Blelloch reduce la profundidad a O(log n), se comprobó que una menor profundidad no garantiza una ejecución más rápida, ya que también influyen los recursos del equipo y los costos del paralelismo.

----

# Conclusiones 
El desarrollo e implementación del algoritmo de Blelloch permitió comprobar cómo una operación aparentemente secuencial, como lo es el cálculo de sumas de prefijos, puede paralelizarse aprovechando la propiedad asociativa de la suma y una estructura de árbol binario. A través de las fases de Up-sweep y Down-sweep, se logró obtener exactamente el mismo resultado que con el método tradicional, pero reduciendo la complejidad de O(n) a O(log n) y conservando un trabajo total óptimo de O(n), lo que respalda lo establecido por la Ley de Brent en el plano teórico.

Sin embargo, al llevar el algoritmo a la práctica en C utilizando OpenMP con un arreglo de 1,048,576 elementos, observamos que la versión secuencial superó en velocidad a la paralela en todas las pruebas realizadas con 1, 2, 4 y 8 hilos. Esto demuestra que una menor profundidad teórica no garantiza automáticamente un menor tiempo de ejecución en la realidad, ya que el algoritmo de Blelloch realiza aproximadamente el doble de operaciones que el recorrido lineal y requiere sincronizar los hilos en cada nivel del árbol, generando un costo de administración (overhead) mayor al tiempo que toma realizar sumas simples de enteros.

Se concluye que el éxito del paralelismo depende en gran medida de la arquitectura y del costo de la operación a realizar. Para arreglos de enteros en una CPU convencional, el enfoque secuencial sigue siendo más conveniente por su bajo costo operativo y mejor aprovechamiento de la memoria caché, mientras que el algoritmo de Blelloch cobra verdadero valor cuando se aplica en arquitecturas masivamente paralelas como las tarjetas gráficas (GPU) o cuando las operaciones entre elementos son mucho más pesadas computacionalmente.
# Referencias 

