#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/sem.h>


#define LARGO_LINEA 		100
#define CANTIDAD_CLIENTES	10
#define	CANT_MIN_CUENTA		10000
#define	CANT_MAX_CUENTA		30000
#define	NOMBRE_ARCHIVO		"bd/cuentas.dat"


int obtenerValorRandomEntre(int, int);


int main(int argc, char const *argv[])
{
	srand(time(NULL));  //semilla para los valores aleatorios

	char	linea[LARGO_LINEA];
	FILE	*cuentas = NULL;
	int 	cantidad = 0;


	remove(NOMBRE_ARCHIVO);	// elimino el archivo para empezar de cero

	cuentas = fopen(NOMBRE_ARCHIVO, "a");	// abro el archivo (si no existe, lo crea)
	if (cuentas != NULL)
	{
		for (int i = 0; i < CANTIDAD_CLIENTES; i++)
		{
			cantidad = obtenerValorRandomEntre(CANT_MIN_CUENTA, CANT_MAX_CUENTA);
			sprintf(linea, "%d|%d\n", (i+1), cantidad);
			fputs(linea, cuentas);
		}
	}

	return 0;
}



// funcion que obtiene un valor aleatorio dentr de un rango
int obtenerValorRandomEntre(int desde, int hasta)
{
	int num;
	int hastaRand = (hasta - desde + 1);
	num = (rand()%hastaRand) + desde;
	return num;
}