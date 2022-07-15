El programa implementa la solución de los lectores-escritores, en la cuál se le permite
leer un recurso a una cantidad arbitraria de lectores (siempre y cuando no se esté 
escribiendo) y se le permite el acceso al recurso a un solo escritor (siempre y cuando
no haya nadie leyendo).

Para compilar y ejecutar el programa:
- Compilar: `gcc Practica3RaulGonzalez.c -fopenmp`
- Ejecutar: `./a.out <num_lectores> <num_escritores>`

**Notas:** 
- Puse tiempo aleatorio al momento de escribir y al momento de leer, ya que
de lo contrario los hilos terminaban de usar el recurso mucho antes de que otro
hilo quisiera acceder a él.

- Puse más tiempo de espera para que un hilo quiera hacer su siguiente lectura
para que den tiempo a los escritores de poder escribir, ya que si no todos los
lectores acababan su ejecución y no dejaban que un escritor modificara el dato.

- Al final tarda tiempo en ejecutarse porque los escritores terminan su ejecución
mucho antes de que los lectores terminen todas sus iteraciones.

