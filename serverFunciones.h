#ifndef _SERVER_FUNCIONES_H
#define _SERVER_FUNCIONES_H

#include "definiciones.h"

#define LONG_BUFFER	500		// largo del buffer
#define	LONG_NOMBRE	100		// largo del nombre del cliente


void reloj(int);
resultadoTransaccion atiendeCliente(int, struct sockaddr_in, int *);
resultadoTransaccion demasiadosClientes(int, struct sockaddr_in);
void error(int, char *);


#endif