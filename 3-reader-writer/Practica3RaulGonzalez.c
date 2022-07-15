#include <stdio.h>
#include <omp.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ESCRITOR_ITERS 5
#define LECTOR_ITERS 15
#define MAX_HILOS 100

// Definimos los semáforos para usarlos en funciones
omp_lock_t wrt; 
omp_lock_t s;
// cantidad de lectores que están leyendo al mismo tiempo
int cantidad_lectores = 0;

// dato para la simulación, es donde leen y escriben
int dato = -1;

int randInt(int n);
void escritor(int idHilo);
void lector(int idHilo);

int main(int argc, char** argv) {
	int numLectores; //cantidad de lectores definida como parámetro
	int numEscritores; //cantidad de escritores definida como parámetro
	
	if(argc < 3) {
		printf("Especifíca el número de lectores seguido del número de escritores\n");
		exit(1);
	}
	sscanf(argv[1], "%i", &numLectores);
	if(numLectores < 1 || numLectores > MAX_HILOS - 1) {
		printf("Número de lectores no válido (%i)\n", numLectores);
		exit(1);
	}
	sscanf(argv[2], "%i", &numEscritores);
	if(numEscritores < 1 || numEscritores > MAX_HILOS - numLectores) {
		printf("Número de escritores no válido (%i)\n", numEscritores);
		exit(1);
	}

	// vamos a tener una cantidad de hilos igual al número de escritores mas el número de lectores
	omp_set_num_threads(numLectores + numEscritores);
	int idHilo;

	// inicializamos la seed para los tiempos random
	time_t t;
	srand(time(&t));
	omp_init_lock(&wrt);
	omp_init_lock(&s);

	// cada hilo ejecuta:
	#pragma omp parallel private(idHilo)
	{
		if (idHilo == 0) {
			printf("Hola soy 0\n");
		}
		idHilo = omp_get_thread_num();
		// Los primeros hilos son los escritores y los escritores y los demás son lectores
		if (idHilo < numEscritores) {
			escritor(idHilo);
		} else {
			lector(idHilo);
		}
	}

	// liberamos los semáforos
	omp_destroy_lock(&wrt);
	omp_destroy_lock(&s);
	printf("Se han terminado de ejecutar todos los hilos\n");
	printf("Adiós!");
}

/*
 * Rutina que ejecuta el escritor
 */
void escritor(int idHilo) {
	// itera la cantidad de veces definida en la tarea
	for(int i = 0; i < ESCRITOR_ITERS; i++) {
		// espera antes de querer volver a escribir
		sleep(randInt(2) + 1);
		printf("Escritor %i: Quiere escribir\n", idHilo);
		// intenta obtener el semáforo, en caso contrario espera
		omp_set_lock(&wrt); //  equivalente a wait(wrt)

		printf("Escritor %i: Escribiendo su id en dato\n", idHilo);
		dato = idHilo; // escribe
		sleep(randInt(3) + 1); //simula tiempo de escritura
		printf("Escritor %i: Terminó de escribir\n", idHilo);

		// libera el semáforo
		omp_unset_lock(&wrt); // equivalente a signal(wrt)
	}
	printf("Escritor %i: Terminó su ejecución\n", idHilo);
}

/*
 * Rutina que ejecuta el lector
 */
void lector(int idHilo) {
	int dataLocal; // registro local para guardar lo que lee
	for (int i = 0; i < LECTOR_ITERS; i++) {
		sleep(randInt(6) + 1); // espera más tiempo para volver a querer leer
		printf("Lector %i: Quiere leer\n", idHilo);

		// actualiza la variable cantidad_lectores garantizando exclusión mutua
		omp_set_lock(&s);
		cantidad_lectores++;
		if (cantidad_lectores == 1) {
			omp_set_lock(&wrt); // si es el primer lector, restringe el acceso a cualquier escritor
		}
		omp_unset_lock(&s);

		printf("Lector %i: Comenzando a leer\n", idHilo);
		dataLocal = dato; // simula la lectura del dato 
		sleep(randInt(3) + 1); //simula tiempo de espera
		printf("Lector %i: Leyendo un %i\n", idHilo, dato);
		printf("Lector %i: terminó de leer\n", idHilo);

		// vuelve a reducir la variable cantidad_lectores porque ya terminó de leer. Garantizamos la exclusión mutua
		omp_set_lock(&s);
		cantidad_lectores--;
		if (cantidad_lectores == 0) {
			omp_unset_lock(&wrt); // si es el último que es estaba leyendo, entonces le da acceso a que los escritores compitan por el recurso
	 	}
		omp_unset_lock(&s);
	}
	printf("Lector %i: terminó su ejecución\n", idHilo);
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
