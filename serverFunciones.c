#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "serverFunciones.h"
#include "definiciones.h"
#include "funciones.h"


#define LARGO_FECHA	25


/**
 * ********************************************************************************************	*
 * @brief "reloj" de actividad																	*
 * Funcion que dibuja una "reloj" indicando actividad											*
 * 																								*
 * @param loop: "contador" que hara que se mueva el rejoj										*
 *  																							*
 * ********************************************************************************************	*/
void reloj(int loop)
{
	if (loop == 0)
		printf("[SERVIDOR] Esperando conexión  ");

	printf("\033[1D");	// introducimos código ANSI para retroceder 2 caracteres
	switch (loop%4)
	{
		case 0: printf("|"); break;
		case 1: printf("/"); break;
		case 2: printf("-"); break;
		case 3: printf("\\"); break;
		default:		// no debemos estar aquí
			break;
	}

	fflush(stdout);		// Actualizamos la pantalla
}


/**
 * ********************************************************************************************	*
 * @brief Funcion para mostrar errores															*
 *   																							*
 * @param code: Codigo de error																	*
 * @param err: Texto a mostrar																	*
 *  																							*
 * ********************************************************************************************	*/
void error(int code, char *err)
{
	char *msg = (char*)malloc(strlen(err)+14);
	sprintf(msg, "Error %d: %s\n", code, err);
	fprintf(stderr, msg);
	exit(1);
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que gestiona el exceso de conexiones entrantes								*
 *   																							*
 * @param socket: conexion que estamos por tratar												*
 * @param addr: informacion de la conexion del cliente. No la usamos pero la dejamos para el 	*
 * futuro																						*
 *  																							*
 * ********************************************************************************************	*/
resultadoTransaccion demasiadosClientes(int socket, struct sockaddr_in addr)
{
	char buffer[LONG_BUFFER];
	int bytecount;		// TODO chk si es necesaria
	resultadoTransaccion respuesta;
	respuesta.exitCode = 0;

	memset(buffer, 0, LONG_BUFFER);

	sprintf(buffer, "Demasiados clientes conectados. Por favor, espere unos minutos\n");

	if((bytecount = send(socket, buffer, strlen(buffer), 0)) == -1)
		error(6, "No puedo enviar información");

	close(socket);

	return respuesta;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que atiende a los clientes entrantes											*
 *  																							*
 * @param socket: descriptor/socket del cliente que se conecta									*
 * @param addr: informacion de la conexion del cliente											*
 * @param cuentas: direccion de memoria del vector que almacena las cuentas de los clientes		*
 *  																							*
 * @return resultadoTransaccion: informacion de la transaccion entre el cliente y el servidor	*
 * 																								*
 * ********************************************************************************************	*/
resultadoTransaccion atiendeCliente(int socket, struct sockaddr_in addr, int *cuentas)
{
	// declaro variables
	char	buffer[LONG_BUFFER];
	char	aux[LONG_BUFFER];
	char	linea[LONG_BUFFER];		// linea que se guarada en los logs
	char	cajero[LONG_NOMBRE];	// nombre del cajero con el que nos estamos conectando
	int		importe;				// cantidad a extraer/depositar
	int		operacion;				// operacion elegida
	int		bytecount;				
	int		seguirLeyendo;			// flag que me indica que el cliente sigue conectado
	int		cuentaAux;				// cantidad en la cuenta
	int		clientId;				// ID del cliente
	time_t	t;
	struct tm *tmp;
	char	fecha[LARGO_FECHA];		// almacena la fecha y la hora
	resultadoTransaccion respuesta;
	int 	id_semaforo;
	FILE	*logs;

	id_semaforo = creo_semaforo(1, CLAVE_SEM_2);
	inicia_semaforo(id_semaforo, NUM_SEMAFORO, VERDE);

	seguirLeyendo = 1;
	respuesta.exitCode = 0;
	respuesta.huboCambios = 0;
	operacion = 0;
	clientId = -1;

	while (seguirLeyendo)
	{
		memset(buffer, 0, LONG_BUFFER);
		if((bytecount = recv(socket, buffer, LONG_BUFFER, 0)) == -1)
			error(5, "No puedo recibir información");

		strcpy(aux, buffer);
		bzero((char *)&buffer, sizeof(buffer));
		operacion = atoi(strtok(aux, "|" ));	// leo la operacion seleccionada
		strcpy(cajero, strtok(NULL, "|" ));		// leo el cajero
		clientId = atoi(strtok(NULL, "|" ));	// leo el ID del cliente
		importe = atoi(strtok(NULL, "|" ));		// leo el importe de extraccion/deposito

		t = time(NULL);
		tmp = localtime(&t);
		strftime(fecha, LARGO_FECHA, "%Y%m%d %H:%M:%S", tmp);

		respuesta.clientId = clientId;

		switch (operacion)
		{
		case EVT_SALIR:
			printf("\nEl cliente %d se ha desconectado del cajero %s...\n", clientId, cajero);
			seguirLeyendo = 0;
			sprintf(linea, "%s | %s:%d | %s | %02d | SALIDA | 0 | %d | %d\n", fecha, inet_ntoa(addr.sin_addr), htons(addr.sin_port), cajero, clientId, cuentas[clientId-1], cuentas[clientId-1]);
			break;

		case EVT_CONSULTAR:
			printf("\nConsultando la cuenta de %d\n", clientId);
			sprintf(aux, "La cuenta tiene ARS %d\n", cuentas[clientId-1]);
			printf(aux);
			send(socket, aux, LONG_BUFFER, 0);
			sprintf(linea, "%s | %s:%d | %s | %02d | CONSULTA | 0 | %d | %d\n", fecha, inet_ntoa(addr.sin_addr), htons(addr.sin_port), cajero, clientId, cuentas[clientId-1], cuentas[clientId-1]);
			break;

		case EVT_EXTRAER:
			printf("\nExtraccion...\n");
			cuentaAux = cuentas[clientId-1];
			printf("La cuenta del cliente %d tiene inicialmente: %d\n", clientId, cuentas[clientId-1]);
			printf("Se le quiere extraer: %d\n", importe);
			if (importe <= cuentas[clientId-1])
			{
				cuentas[clientId-1] = cuentas[clientId-1] - importe;
				sprintf(aux, "Monto despues de la extraccion: %d\n", cuentas[clientId-1]);
				printf(aux);
				send(socket, aux, LONG_BUFFER, 0);
				respuesta.huboCambios = 1;
			}
			else
			{
				sprintf(aux, "No se pudo realizar la extraccion. El monto es superior a lo que tiene la cuenta\n");
				printf(aux);
				send(socket, aux, LONG_BUFFER, 0);
				respuesta.huboCambios = 0;
			}
			sprintf(linea, "%s | %s:%d | %s | %02d | EXTRACCION | %d | %d | %d\n", fecha, inet_ntoa(addr.sin_addr), htons(addr.sin_port), cajero, clientId, importe, cuentaAux, cuentas[clientId-1]);
			break;

		case EVT_DEPOSITAR:
			printf("\nDeposito...\n");
			cuentaAux = cuentas[clientId-1];
			printf("La cuenta del cliente %d tiene inicialmente: %d\n", clientId, cuentas[clientId-1]);
			printf("Se le quiere depositar: %d\n", importe);
			cuentas[clientId-1] = cuentas[clientId-1] + importe;
			sprintf(aux, "Monto despues del deposito: %d\n", cuentas[clientId-1]);
			printf(aux);
			send(socket, aux, LONG_BUFFER, 0);
			respuesta.huboCambios = 1;
			sprintf(linea, "%s | %s:%d | %s | %02d | DEPOSITO | %d | %d | %d\n", fecha, inet_ntoa(addr.sin_addr), htons(addr.sin_port), cajero, clientId, importe, cuentaAux, cuentas[clientId-1]);
			break;
		
		default:
			printf("ERROR: Se ha producido un error. No se debe pasar por aca.\n");
			break;
		}

		respuesta.montoActualizado = cuentas[clientId-1];

		espera_semaforo(id_semaforo, NUM_SEMAFORO);

			logs = fopen(ARCHIVO_LOGS, "a");
			if (logs != NULL)
			{
				fputs(linea, logs);
				fclose(logs);
			}
			else
				error(10, "Error al abrir el archivo de logs");

		levanta_semaforo(id_semaforo, NUM_SEMAFORO);

	}

	close(socket);
	return respuesta;
}

