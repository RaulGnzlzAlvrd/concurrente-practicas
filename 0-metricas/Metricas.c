#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define LIM_SUP 2000000000

/*
*Codigo auxiliar para el ejercicio de métricas.
*Realiza la suma de los primeros 1,000,000,000 números naturales de forma paralela
*y lleva registro del tiempo de ejecucion
*/



int main(int argc, char** argv){
	struct timeval inicio, fin;//nos permiten medir el tiempo de ejecucion
	gettimeofday(&inicio, NULL);//guarda el tiempo al inicio del programa
	int tiempo;
	
	int MAX_HILOS=100; //limite superior (arbitrario) del numero de hilos
	int numHilos, idHilo;
	
	//verifica el numero de hilos que se paso como parametro
	if (argc < 2){
		printf("por favor especifique el numero de hilos\n");
		exit(1);
	}
	sscanf(argv[1], "%i", &numHilos);
	if (numHilos < 1 || numHilos > MAX_HILOS){
		printf("Numero de hilos no valido (%i)\n", numHilos);
		exit(1);
	}
	
	omp_set_num_threads(numHilos);
	long int suma, sumaHilo;
	suma=0;
	
	//inicia seccion paralela
	#pragma omp parallel private(idHilo, sumaHilo)
    {
		idHilo = omp_get_thread_num();
		if (idHilo==0){
			printf ("iniciando calculo con %i hilos\n", omp_get_num_threads());
		}
		
		//cada hilo realiza una suma parcial
		sumaHilo=0;
		int i;
		for (i=idHilo;i<LIM_SUP;i+=numHilos){
			sumaHilo+=i;
		}
		
		//indica que la siguiente seccion se realiza con acceso exclusivo
		#pragma omp critical
		{
			//cada hilo actualiza la suma total con su resultado parcial
			suma+=sumaHilo;
		}
	}//fin de seccion paralela

	gettimeofday(&fin, NULL); //guarda el tiempo al final del programa
	tiempo = (fin.tv_sec - inicio.tv_sec)* 1000000 + (fin.tv_usec - inicio.tv_usec);
	printf("suma: %li  \ntiempo de ejecucion: %i microsegundos\n",suma, tiempo); //imprime resultados
}
