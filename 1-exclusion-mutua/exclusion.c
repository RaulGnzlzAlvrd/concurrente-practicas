#include <stdio.h>
#include <stdbool.h>
#include <omp.h>

#define LIM_SUP 1000
#define ITERS 20
#define NUM_THREADS 2

void sumTo(int n, long int* res);
void algoritmoIngenuoI();
void algoritmoDekker();
void algoritmoPeterson();
void algoritmoKessels();

int main() {

	printf("\nComienza ejecución con Algoritmo Dekker\n\n");
	// La ejecución se detiene hasta que terminan todos los hilos dentro de la función.
	// así aseguramos que el siguiente algoritmo se ejecuta hasta que termina este.
	algoritmoDekker(); 
	printf("\nTermina ejecución con Algoritmo Dekker\n\n");


	printf("\nComienza ejecución con Algoritmo Peterson\n\n");
	algoritmoPeterson();
	printf("\nTermina ejecución con Algoritmo Peterson\n\n");


	printf("\nComienza ejecución con Algoritmo Kessels\n\n");
	algoritmoKessels();
	printf("\nTermina ejecución con Algoritmo Kessels\n\n");


	// Este algoritmo lo usé para hacerme una idea de como organizar los otros
	// No cumple ser de exclusión mutua, y se puede dar una ejecución donde
	// el proceso 1 y el 0 están al mismo tiempo en su sección crítica.
	// printf("\nComienza ejecución con Algoritmo ingenuo I\n\n");
	// algoritmoIngenuoI();
	// printf("\nTermina ejecución con Algoritmo ingenuo I\n\n");
}

/*
 * Ejecución del algoritmo de Dekker, este sí es un
 * algoritmo de exclusión mutua.
 *
 * Todos los algoritmos tendrán una estructura similar:
 * - Primero definimos nuestras variables compartidas por todos los hilos
 * - Dentro de cada hilo definimos las variables que se usaran para el protocolo (algoritmo de 
 *   exclusión
 * - Se entra a un ciclo para que cada hilo entre 20 veces a su sección crítica
 * - La primera parte del algoritmo es el preprotocol
 * - En la sección crítica se hace una prueba de trabajo sumando números
 * - Luego ejecutamos la parte final del algoritmo en postprotocol
 */
void algoritmoDekker() {
	omp_set_num_threads(NUM_THREADS);

	// shared variables
	int turn = 0; // De quién es el turno en caso de que ambos quieran entrar (a su sección crítica
	bool flag[] = {false, false}; // Indica si el hilo i quiere entrar

	int idHilo;
	#pragma omp parallel private(idHilo)
	{
		// Inicializamos variables que se usan para cada hilo
		idHilo = omp_get_thread_num();
		long int result;
		int otro;

		// Tiene que hacer 20 iteraciones cada hilo
		for(int i = 0; i < ITERS; i++) {
			result = 0;

			// preprotocol
			otro = (idHilo + 1) % 2; // Obtenemos el id del otro hilo
			flag[idHilo] = true; // Decimo que queremos entrar a la sección
			while (flag[otro]) { // Si el otro quiere entrar a la sección
				if (turn == otro) { // Hacemos un desempate usando el turno
					flag[idHilo] = false;
					while (turn != idHilo); // Esperamos a que sea nuestro turno
					flag[idHilo] = true;
				}
			} // si el otro no quiere entrar, entonces entramos directamente

			// sección crítica
			printf("Hilo %i: entró a sección crítica\n", idHilo);
			sumTo(LIM_SUP, &result);
			printf("Hilo %i: calculó resultado %li\n", idHilo, result);

			// postprotocol
			turn = otro; // Cedemos el turno
			flag[idHilo] = false; // Indicamos que ya no queremos entrar.
			printf("Hilo %i: salió de sección crítica\n", idHilo);
		}
	}
}

/*
 * Ejecución del algoritmo de Peterson, este sí es un
 * algoritmo de exclusión mutua.
 */
void algoritmoPeterson() {
	omp_set_num_threads(NUM_THREADS);

	// shared variables
	int turn = 0;
	bool flag[] = {false, false};

	int idHilo;
	#pragma omp parallel private(idHilo)
	{
		idHilo = omp_get_thread_num();
		long int result;
		int otro;

		// Tiene que hacer 20 iteraciones cada hilo
		for(int i = 0; i < ITERS; i++) {
			result = 0;

			// preprotocol
			otro = (idHilo + 1) % 2;
			flag[idHilo] = true; // Indicamos que queremos entrar a nuestra sección critica
			turn = otro; // Cedemos el turno al otro
			while (flag[otro] && turn == otro); // Si el otro también quiere entrar, entonces se hace el desempate con el turno.

			// sección crítica
			printf("Hilo %i: entró a sección crítica\n", idHilo);
			sumTo(LIM_SUP, &result);
			printf("Hilo %i: calculó resultado %li\n", idHilo, result);

			// postprotocol
			flag[idHilo] = false; // Anunciamos que dejamos la sección crítica
			printf("Hilo %i: salió de sección crítica\n", idHilo);
		}
	}
}

/*
 * Ejecución del algoritmo de Kessels, este sí es un
 * algoritmo de exclusión mutua.
 */
void algoritmoKessels() {
	omp_set_num_threads(NUM_THREADS);

	// shared variables
	int turn[] = {0,1};
	bool flag[] = {false, false};

	int idHilo;
	#pragma omp parallel private(idHilo)
	{
		idHilo = omp_get_thread_num();
		long int result;
		int otro;
		int local;

		// Tiene que hacer 20 iteraciones cada hilo
		for(int i = 0; i < ITERS; i++) {
			result = 0;

			// preprotocol
			otro = (idHilo + 1) % 2;
			flag[idHilo] = true;
			// Si id = 0 estoy cambiando turn[0] a lo mismo que tenía el proceso 1
			// Si id = 1 estoy cambiando turn[1] a lo contrario que tenía el proceso 0
			// Los procesos están compitiendo por que se cumpla su condición de entrada
			local = (turn[otro] + idHilo) % 2; 
			turn[idHilo] = local;
			// Si el otro no quiere entrar entonces entro, en caso contrario hay
			// desempate según las siguientes condiciones:
			// Si id = 0 entonces para entrar se debe cumplir: local == turn[otro]
			// Si id = 1 entonces para entrar se debe cumplir: local != turn[otro]
			while(flag[otro] == true && local == (turn[otro] + idHilo) % 2);

			// sección crítica
			printf("Hilo %i: entró a sección crítica\n", idHilo);
			sumTo(LIM_SUP, &result);
			printf("Hilo %i: calculó resultado %li\n", idHilo, result);

			// postprotocol
			flag[idHilo] = false;
			printf("Hilo %i: salió de sección crítica\n", idHilo);
		}
	}
}

/*
 * Ejecución del primer algoritmo que vimos
 * Prueba con el Algoritmo ingenuo I
 *
 * No es un algoritmo de exclusión mutua.
 */
void algoritmoIngenuoI() {
	omp_set_num_threads(NUM_THREADS);

	// shared variables
	bool flag = false;

	int idHilo;
	#pragma omp parallel private(idHilo)
	{
		idHilo = omp_get_thread_num();
		long int result;

		// Tiene que hacer 20 iteraciones cada hilo
		for(int i = 0; i < ITERS; i++) {
			result = 0;

			// preprotocol
			while(flag);
			// simulamos que el hilo 1 se tarda mucho en cambiar flag
			// a true para mostrar que no hay mutex en el algoritmo
			if (idHilo == 1) {
				sumTo(LIM_SUP, &result);
				result = 0;
			}
			flag = true;

			// sección crítica
			printf("Hilo %i: entró a sección crítica\n", idHilo);
			sumTo(LIM_SUP, &result);
			printf("Hilo %i: calculó resultado %li\n", idHilo, result);

			// postprotocol
			flag = false;
			printf("Hilo %i: salió de sección crítica\n", idHilo);
		}
	}
}

/*
 * Calcula la suma de los primeros n números
 * naturales (comenzando con el 1)
 * y los guarda en res
 *
 * Es solamente trabajo para forzar retardos.
 */
void sumTo(int n, long int* res) {
	for(int i = 1; i <= n; i++) {
		*res += i;
	}
}

