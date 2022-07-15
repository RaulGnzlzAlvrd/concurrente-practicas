#include <stdio.h>
#include <omp.h>
#include <stdbool.h>
#include <unistd.h> // Para el sleep
#include <stdlib.h> // Para números aleatorios
#include <time.h> // Para inicializar la semilla de aleatorios

// Número de iteraciones por defecto que hace cada filósofo
#define ITERS 5
#define MAX_HILOS 100 // Límite de filósofos

int randInt(int n);

int main(int argc, char** argv) {
    int numFilosofos;
    int numIteraciones;

    // El número de filósofos se tiene que dar como entrada al programa
    if(argc < 2) {
	printf("[*] Especifica el número de filósofos\n");
	exit(1);
    }
    sscanf(argv[1], "%i", &numFilosofos);
    if(numFilosofos < 1 || numFilosofos > MAX_HILOS) {
	printf("[*] Número de filósofos no válido (%i)\n", numFilosofos);
	exit(1);
    }
    // Podemos cambiar el número de iteraciones que va a realizar cada filósofo
    if(argc == 3) {
	sscanf(argv[2], "%i", &numIteraciones);
	printf("[*] Se realizarán %i iteraciones por filósofo\n", numIteraciones);
    } else {
	numIteraciones = ITERS;
	printf("[*] Se realizarán %i (default) iteraciones por filósofo\n", numIteraciones);
    }

    // Inicializamos los hilos
    omp_set_num_threads(numFilosofos);
    int idHilo;

    // Inicializamos las funciones de random
    time_t t;
    srand(time(&t));

 
    // Inicializamos los parámetros del algoritmo
    // Debe haber a lo más n-1 filósofos sentados al mismo tiempo
    int sillasLibres = numFilosofos - 1; 
    // Usamos un arreglo de booleanos para representar si el palillo está disponible
    // o no.
    // Verdadero significa que está disponible, falso que otro filósofo lo está
    // usando.
    bool palillos[numFilosofos];
    for(int i = 0; i < numFilosofos; i++) {
	palillos[i] = true; // Los palillos comienzan disponibles
    }

    // Cada filosofo ejecuta:
    #pragma omp parallel private(idHilo)
    {
	// Su id o número de asiento
	idHilo = omp_get_thread_num();

	// Calcumos cuál es el id de su palillo izquierdo y derecho
	// dependiendo de su asiento
	int derecho = idHilo;
	int izquierdo = (idHilo + 1) % numFilosofos;

	// Flag que nos va a ayudar para saber si estamos esperando a
	// que se desocupe un recurso.
	bool esperando = true;

	for(int i = 0; i < numIteraciones; i++) {
	    printf("[*] Filósofo %i: Está pensando\n", idHilo);
	    sleep(randInt(4)+1); // Esperamos entre 1 y 5 segundos
	    printf("[*] Filósofo %i: Tiene hambre\n", idHilo);

	    // Intentamos ganar un asiento
	    while(esperando) {
		#pragma omp critical
		{
		    // Solo si hay un asiento disponible lo tomamos y dejamos
		    // de esperar.
		    if(sillasLibres > 0) {
			sillasLibres--;
			esperando = false;
			printf("Filósofo %i: Tomó asiento\n", idHilo);
		    }
		}
	    }
	    esperando = true; // Ahora esperaremos por un palillo

	    // Intentamos ganar el palillo derecho
	    while(esperando) {
		#pragma omp critical
		{
		    // Solo si está disponible el palillo lo tomamos y dejamos
		    // de esperar.
		    if(palillos[derecho]) {
			palillos[derecho] = false;
			esperando = false;
			printf("Filósofo %i: Tomó palillo derecho %i\n", idHilo, derecho);
		    }
		}
	    }
	    esperando = true;

	    // Intentamos ganar el palillo izquierdo
	    while(esperando) {
		#pragma omp critical
		{
		    // Igual al caso anterior
		    if(palillos[izquierdo]) {
			palillos[izquierdo] = false;
			esperando = false;
			printf("Filósofo %i: Tomó palillo izquierdo %i\n", idHilo, izquierdo);
		    }
		}
	    }
	    esperando = true;

	    // Tenemos ambos palillos así que comemos
	    printf("[*] Filósofo %i: Está comiendo\n", idHilo);
	    sleep(randInt(4)+1); // Esperamos entre 1 y 5 segundos
	    // Terminamos de comer

	    // Liberamos los recursos y anunciamos nuestra salida
	    #pragma omp critical
	    {
		palillos[derecho] = true;
		palillos[izquierdo] = true;
		sillasLibres++;

		printf("Filósofo %i: Libera palillo derecho %i\n", idHilo, derecho);
		printf("Filósofo %i: Libera palillo izquierdo %i\n", idHilo, izquierdo);
		printf("Filósofo %i: Libera silla\n", idHilo);
		printf("[*] Filósofo %i: Terminó de comer\n", idHilo);
	    }
	}
	printf("Filósofo %i: Termina sus iteraciones\n", idHilo);
    }
}

/*
 * Genera un número entero aleatorio entre 0 (incluido) y n (excluido)
 */
int randInt(int n) {
    double randNorm = ((double) rand()) / ((double) RAND_MAX);
    int range = (float) n;
    int randTo = (int) (range * randNorm);
    return randTo;
}
