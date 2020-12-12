//#include <sys/types.h>
//#include <sys/socket.h>
#include <sys/un.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>




int abrirSocket(char *hostServidor, char *hostPort)
{
	int					conexion;
	int					puerto;
	struct hostent		*servidor;	// declaración de la estructura con información del host
	struct sockaddr_in	cliente;	// declaración de la estructura con información para la conexión

	// asignacion
	servidor = gethostbyname(hostServidor); 
	puerto   = (atoi(hostPort));
	
	if(servidor == NULL)
	{ //Comprobación
		printf("Host erróneo\n");
		return -1;
	}

	// rellena toda la estructura de 0's. La función bzero() es como memset() pero inicializando a 0 todas la variables
	bzero((char *)&cliente, sizeof((char *)&cliente));	
	
	cliente.sin_family	= AF_INET;			// asignacion del protocolo
	cliente.sin_port	= htons(puerto);	// asignacion del puerto
	bcopy((char *)servidor->h_addr, (char *)&cliente.sin_addr.s_addr, sizeof(servidor->h_length));
	//bcopy(); copia los datos del primer elemendo en el segundo con el tamaño máximo del tercer argumento.

	conexion = socket(AF_INET, SOCK_STREAM, 0);	// asignación del socket


	//cliente.sin_addr = *((struct in_addr *)servidor->h_addr); //<--para empezar prefiero que se usen
	//inet_aton(argv[1],&cliente.sin_addr); //<--alguna de estas dos funciones
	if(connect(conexion,(struct sockaddr *)&cliente, sizeof(cliente)) < 0)
	{ //conectando con el host
		printf("Error conectando con el host\n");
		close(conexion);
		return -1;
	}
	printf("Conectado con %s:%d\n",inet_ntoa(cliente.sin_addr),htons(cliente.sin_port));
	//inet_ntoa(); está definida en <arpa/inet.h>



	return conexion;
}