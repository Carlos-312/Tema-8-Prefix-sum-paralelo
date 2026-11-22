# Tema-8-Prefix-sum-paralelo

---

# Integrantes
- [323255343] - Gil Isaias Kevin
- [323273613] - Sánchez Ledesma Paulina
- [número de cuenta] - Nombre
- [número de cuenta] - Nombre

---

# Contexto y teoría

**Concepto:** El tema central es la resolución computacional de la suma de prefijos de forma paralela mediante el algoritmo de Blelloch (también conocido como *scan*).

**Objetivo:** El propósito de la exposición es mostrar cómo calcular sumas acumuladas en paralelo manteniendo una eficiencia de trabajo de `O(n)` y reduciendo drásticamente la profundidad computacional a `O(log n)`.

# ¿Qué es Prefix Sum?

Prefix Sum, también conocido como suma de prefijos o suma acumulada, es una operación que permite calcular progresivamente las sumas de los elementos de un arreglo. Su objetivo es obtener, para cada posición, la suma de los elementos anteriores o hasta la posición actual.

Se utiliza en diferentes áreas de la computación, como el procesamiento de datos, imágenes y algoritmos paralelos. Además, no se limita únicamente a la suma, ya que también puede aplicarse a operaciones asociativas, como la multiplicación, el máximo o el mínimo. Esta propiedad permite reorganizar los cálculos sin modificar el resultado.

**Prefix Sum Paralelo** 
Tradicionalmente, las sumas acumuladas se calculan de manera secuencial, donde cada operación depende del resultado anterior. Esta dependencia dificulta que los cálculos se realicen simultáneamente.

Para superar esta limitación, Prefix Sum Paralelo reorganiza las operaciones para distribuirlas entre varios hilos o procesadores. En lugar de calcular las sumas consecutivamente, utiliza resultados parciales que pueden obtenerse al mismo tiempo y combinarse posteriormente.Esto es posible gracias a la propiedad asociativa de la suma, que permite agrupar los elementos de distintas maneras sin alterar el resultado.

Entre los algoritmos utilizados se encuentran Hillis-Steele y Blelloch. Aunque ambos reducen la profundidad computacional a \(O(\log n)\), Blelloch destaca porque mantiene un trabajo total de \(O(n)\), logrando una mayor eficiencia teórica.

**Algoritmo de Blelloch**
El algoritmo de Blelloch utiliza una estructura de árbol binario para organizar las operaciones en diferentes niveles, permitiendo ejecutar simultáneamente los cálculos independientes.

Su funcionamiento se divide en dos fases:
- Up-sweep (reducción ascendente): consiste en combinar los elementos del arreglo para obtener sumas parciales de grupos cada vez más grandes, hasta calcular la suma total. En cada nivel, las operaciones de grupos diferentes pueden ejecutarse simultáneamente.
- Down-sweep (distribución descendente): comienza reemplazando por cero el valor de la raíz y distribuye las sumas parciales a través del árbol. De esta manera, cada posición obtiene la suma de los elementos anteriores, generando el Prefix Sum exclusivo.

**Independencia de operaciones y OpenMP**
Una característica fundamental de Blelloch es que las operaciones de un mismo nivel son independientes, porque trabajan sobre posiciones diferentes del arreglo.Para organizar estos cálculos se utiliza un desplazamiento llamado stride o salto, que determina qué elementos deben combinarse en cada nivel. Durante Up-sweep, este salto aumenta progresivamente, mientras que en Down-sweep disminuye.

Esta característica permite implementar el algoritmo mediante OpenMP, utilizando la directiva #pragma omp parallel for para distribuir las operaciones entre diferentes hilos.Al trabajar sobre posiciones independientes, se evitan las condiciones de carrera dentro de cada nivel. Sin embargo, los niveles consecutivos sí presentan dependencias, por lo que es necesario sincronizar los hilos antes de continuar con el siguiente.

**Relación con la Ley de Brent, Trabajo y Profundidad:**
La Ley de Brent establece que un algoritmo con un trabajo total `T1` y una profundidad `T_inf` puede ejecutarse en `p` procesadores con un tiempo limitado por `Tp <= (T1 / p) + T_inf`. 
Aunque el algoritmo de suma secuencial tradicional realiza un trabajo lineal (`O(n)`), su naturaleza es estrictamente dependiente (el paso actual depende del anterior), lo que deja su profundidad también en `O(n)`. Otros algoritmos paralelos básicos (como el de Hillis-Steele) bajan la profundidad a `O(log n)` pero desperdician ciclos aumentando el trabajo a `O(n log n)`. 
El algoritmo de Blelloch brilla porque es *work-efficient* (óptimo en trabajo): reorganiza las operaciones en forma de árbol, conservando el trabajo asintótico en `O(n)` y alcanzando una profundidad logarítmica de `O(log n)`.

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

El hijo izquierdo recibe el valor de su padre.

El hijo derecho recibe el valor del padre más la suma original del hijo izquierdo.

Paso 1. Primer nivel

Las sumas originales de las ramas eran 10 y 26. Comenzamos con cero:

```
Izquierda = 0

Derecha = 0 + 10 = 10

Resultado: [0, 10]
```

Paso 2. Segundo nivel

Distribuimos los valores hacia los siguientes nodos:

```
Izquierda: 0 y 0 + 3 = 3

Derecha: 10 y 10 + 11 = 21

Resultado: [0, 3, 10, 21]
```

Paso 3. Tercer nivel

Repetimos el procedimiento hasta llegar a las posiciones originales:

```
Pareja -------- Izquierda -------- Derecha

1 y 2 ------------ 0 --------------- 1

3 y 4 ------------ 3 --------------- 6

5 y 6 ------------ 10 -------------- 15

7 y 8 ------------ 21 -------------- 28
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

Sin embargo, debemos esperar a que termine un nivel antes de comenzar el siguiente, ya que los resultados de un nivel se utilizan en el siguiente. Algo importante es que las operaciones de un mismo nivel pueden ejecutarse en paralelo, mientras que entre niveles deben respetarse las dependencias.

---

# Trabajo y profundidad

Para analizar el rendimiento de un algoritmo paralelo se utilizan dos conceptos:

Trabajo: representa la cantidad total de operaciones que realiza el algoritmo.

Profundidad: representa la cantidad de niveles de operaciones dependientes que deben ejecutarse.


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
