#include <sys/un.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_QUEUE	5		// numero maximo de clientes encolados antes de rechazar conexiones


/**
 * ********************************************************************************************	*
 * @brief Funcion abre un socket nuevo															*
 *  																							*
 * @param port: puerto donde tiene que escuchar el socket										*
 *  																							*
 * @return socket/descriptor que se creo														*
 * 																								*
 * ********************************************************************************************	*/
int abrirSocket(char *port)
{
	struct sockaddr_in	servidor;
	int					puerto;
	int					conexionServidor;

	puerto = atoi(port);
	conexionServidor = socket(AF_INET, SOCK_STREAM, 0);	// creamos el socket
	if (conexionServidor < 0)
		return -1;
	
	bzero((char *)&servidor, sizeof(servidor));			// llenamos la estructura de 0's
	

	// Se obtiene el servicio del fichero /etc/services
	//  Puerto = getservbyname (Servicio, "tcp");
	//  if (Puerto == NULL)
	//  	return -1;


	/*
	* Se rellenan los campos de la estructura servidor, necesaria
	* para la llamada a la funcion bind()
	*/
	servidor.sin_family			= AF_INET;			// asignamos el protocola a la estructura
	servidor.sin_port			= htons(puerto);	// asignamos el puerto
	servidor.sin_addr.s_addr	= INADDR_ANY;		// esta macro especifica nuestra dirección

	if(bind(conexionServidor, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
	{ //asignamos un puerto al socket
		printf("Error al asociar el puerto a la conexion\n");
		close(conexionServidor);
		return 1;
	}	// TODO chk los distintos errores de salida y manejarlos

	listen(conexionServidor, MAX_QUEUE);	// estamos a la escucha
	printf("A la escucha en el puerto %d\n", ntohs(servidor.sin_port));

	return conexionServidor;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion acepta nuevas conexiones														*
 *  																							*
 * @param conexionServidor: socket del servidor que esta escuchando								*
 * @param cliente: informacion de la conexion del cliente										*
 *  																							*
 * @return la conexion al cliente aceptada														*
 * 																								*
 * ********************************************************************************************	*/
int aceptarConexionCliente(int conexionServidor, struct sockaddr_in *cliente)
{
	socklen_t			longCliente;			// variable que contendrá la longitud de la estructura
	int					conexionCliente;
	struct sockaddr_in	clienteAux;

	/*
	* La llamada a la funcion accept requiere que el parametro longCliente contenga 
	* inicialmente el tamano de la estructura Cliente que se le pase.
	* A la vuelta de la funcion, esta variable contiene la longitud de la informacion
	* util devuelta en Cliente
	*/
	longCliente = sizeof(clienteAux);		//Asignamos el tamaño de la estructura a esta variable
	conexionCliente = accept(conexionServidor, (struct sockaddr *)&clienteAux, &longCliente);	//Esperamos una conexion
	if(conexionCliente < 0)
	{
		printf("Error al aceptar trafico\n");
		// close(conexionServidor);
		return -1;
	}

	printf("\nConectando con %s:%d\n", inet_ntoa(clienteAux.sin_addr),htons(clienteAux.sin_port));

	cliente->sin_addr = clienteAux.sin_addr;
	cliente->sin_port = clienteAux.sin_port;

	/*
	* Se devuelve el descriptor en el que esta "enchufado" el cliente.
	*/
	return conexionCliente;
}