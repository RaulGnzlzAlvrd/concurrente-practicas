El programa implementa la solución del portero, en la cuál solo se le permite acceder a `n-1` filósofos.

Para compilar y ejecutar el programa:
- Compilar: `gcc Practica2RaulGonzalez.c -fopenmp`
- Ejecutar: `./a.out <num_filosofos> [num_iteraciones]`

El parámetro de número de iteraciones es opcional y por defecto son 5 iteraciones.

Si se quiere una ejecución con 10 filósofos sería: `./a.out 10`.
Con 10 filósofos y 3 iteraciones cada uno: `./a.out 10 3`.

**Nota:** El programa imprime también cuando un filósofo ocupa una silla o cuando un filósofo toma un palillo,
por lo que si solo se quiere ver los mensajes estrictamente requeridos (marcados con `[*]`) usar: `./a.out 10 | grep \[\*\]`
