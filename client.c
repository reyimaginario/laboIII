/**
 * ******************************************************************** *
 * @file client.c														*
 * @brief Cliente de la aplicacion										*
 * Este programa simula de manera sencilla y simple un cajero (ATM)		*
 * 																		*
 * @author Gonzalo Caba													*
 * @version 0.3Beta														*
 * @date 2020/12/02														*
 * 																		*
 * Historial de cambios:												*
 *	v0.1	- Version inicial: Se crea el socket						*
 *	v0.2	- Se implementa el menu										*
 *	v0.3	- Se agrega el ID de cliente y el hostname					*
 *  																	*
 * ******************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include "socketCliente.h"
#include "definiciones.h"


#define LONG_MJE	500
#define	LONG_NOMBRE	100


int menu();


int main(int argc, char **argv){
	if(argc < 4)
	{
		printf("<host> <puerto> <clienteID>\n");
		return -1;
	}	

	int		opcion;
	int		conexion;
	int		seguirEnviando;
	char	buffer[LONG_MJE];
	char	hostname[LONG_NOMBRE];
	int		importe;
	int		clienteId;

	seguirEnviando = 1;
	opcion = 0;
	importe = 0;
	clienteId = atoi(argv[3]);

	if (clienteId < 1 || clienteId > 10)
	{
		printf("El ID del cliente debe estar entre 1 y 10\n");
		fflush(stdin);
		printf("Ingrese el ID del cliente nuevamente: ");
		while( (scanf(" %d", &clienteId) != 1)	|| (clienteId < 1) || (clienteId > 10) )
		{
			fflush(stdin);
			printf("El ID del cliente debe estar entre 1 y 10\n");
			printf("Ingrese el ID del cliente nuevamente: ");
		}
	}

	conexion = abrirSocket(argv[1], argv[2]);	// abro la conexion con el servidor
	gethostname(hostname, LONG_NOMBRE);			// obtengo el nombre de la maquina

	if (conexion != -1)
	{
		while (seguirEnviando)
		{
			opcion = menu();
			switch (opcion)
			{
				case EVT_SALIR:
					sprintf(buffer, "%d|%s|%d|%d", EVT_SALIR, hostname, clienteId, importe);
					printf("\nSaliendo...\n");
					send(conexion, buffer, LONG_MJE, 0); //envio
					bzero(buffer, LONG_MJE);
					seguirEnviando = 0;
					break;

				case EVT_CONSULTAR:
					sprintf(buffer, "%d|%s|%d|%d", EVT_CONSULTAR, hostname, clienteId, importe);
					send(conexion, buffer, LONG_MJE, 0); //envio
					bzero(buffer, LONG_MJE);
					recv(conexion, buffer, LONG_MJE, 0); //recepción
					printf("%s", buffer);
					break;

				case EVT_EXTRAER:
					fflush(stdin);
					printf("Cual es el monto que desea extraer?: ");
					scanf("%d", &importe);
					sprintf(buffer, "%d|%s|%d|%d", EVT_EXTRAER, hostname, clienteId, importe);
					send(conexion, buffer, LONG_MJE, 0); //envio
					bzero(buffer, LONG_MJE);
					recv(conexion, buffer, LONG_MJE, 0); //recepción
					printf("%s", buffer);
					break;

				case EVT_DEPOSITAR:
					fflush(stdin);
					printf("Cual es el monto que desea depositar?: ");
					scanf("%d", &importe);
					sprintf(buffer, "%d|%s|%d|%d", EVT_DEPOSITAR, hostname, clienteId, importe);
					send(conexion, buffer, LONG_MJE, 0); //envio
					bzero(buffer, LONG_MJE);
					recv(conexion, buffer, LONG_MJE, 0); //recepción
					printf("%s", buffer);
					break;

				default:
					printf("ERROR: Se ha producido un error. No se debe pasar por aca.\n");
					break;
			}

		}

		close(conexion);

	}
	else
	{
		return -1;
	}
	
	return 0;

}



int menu()
{
	int opcion;

	printf("\nMenu\n");
	printf("----\n");
	printf("  1. Consular\n");
	printf("  2. Extraccion\n");
	printf("  3. Depositar\n");
	printf("  0. Salir\n\n");
	printf("Elija opcion: ");

	while( (scanf(" %d", &opcion) != 1)	|| (opcion < 0) || (opcion > 3))
	{
		fflush(stdin);
		printf("No valido\n\n");
		printf("Elija opcion: ");
	}
	return opcion;
}
