/**
 * ******************************************************************** *
 * @file server.c														*
 * @brief Servidor de la aplicacion										*
 * Este programa simula de manera sencilla y simple un cajero (ATM)		*
 * 																		*
 * @author Gonzalo Caba													*
 * @version 0.6Beta														*
 * @date 2020/12/06														*
 * 																		*
 * Historial de cambios:												*
 * 	v0.1   - Version inicial: Se crea el socket							*
 *  v0.2   - Se pueden mandar varios mensajes entre el servidor y el	*
 *           cliente antes de cerrar el socket							*
 *  v0.3   - Se aceptan varias conexiones simultaneas					*
 *  v0.4.1 - Lee montos de una cuenta de un archivo						*
 *  v0.4.2 - Actualiza los valores (memoria compartida)					*
 *  v0.5   - Guarda logs												*
 *  v0.6   - No permite retirar mas de lo que tiene la cuenta			*
 * 																		*
 * ******************************************************************** */

#include <netdb.h>
#include <sys/wait.h>
#include "socketServidor.h"
#include "definiciones.h"
#include "serverFunciones.h"
#include "funciones.h"


#define MAX_CHILDS		3		// numero maximos de hijos
#define TIMEOUT_MS		500000	// timeout para aceptar una nueva connecxion


void cargoCuentas(int *);


int main(int argc, char **argv){
	if(argc < 2)
	{ //Especifica los argumentos
		printf("%s <puerto>\n",argv[0]);
		return 1;
	}

	// declaramos las variables
	int		conexionServidor;		// descriptor/socket del servidor
	int		conexionCliente;		// descriptor/socket del cliente
	struct sockaddr_in clienteData;	// estructura que contiene la info del cliente
	fd_set	rfds;					// conjunto de descriptores a vigilar
	struct timeval tv;				// para el timeout del accept
	int		childcount;				// indica cuantos clientes hay conectados
	int		childpid;
	int		pidstatus;
	int		activated;				// flag para conrtar el servidor
	int		loop;					// varaible que "hace girar el reloj"
	int		*cuentasClientes;		// aca se almacenaran los montos de las cuentas de los clientes
	resultadoTransaccion respuesta;
	int id_semaforo;
	int id_memoria;

	childcount = 0;
	activated = 1;
	loop = 0;
	id_semaforo = creo_semaforo(1, CLAVE_SEM_1);
	inicia_semaforo(id_semaforo, NUM_SEMAFORO, VERDE);
	cuentasClientes = (int*) creo_memoria(sizeof(int)*CANTIDAD_CLIENTES, &id_memoria, CLAVE_BASE);
	cargoCuentas(cuentasClientes);


	conexionServidor = abrirSocket(argv[1]);
	if (conexionServidor == -1)
	{
		printf ("No se puede abrir socket servidor\n");
		return -1;
	}


	while (activated)
	{
		reloj(loop);

		// select() se carga el valor de rfds
		FD_ZERO(&rfds);						// nos vacía el puntero (rfds), de forma que estamos indicando que no nos interesa ningún descriptor de fichero.
		FD_SET(conexionServidor, &rfds);	// mete el descriptor que le pasamos (conexionServidor) al puntero rfds. De esta forma estamos indicando que tenemos interes en ese descriptor.

		// select() se carga el valor de tv
		tv.tv_sec = 0;
		tv.tv_usec = TIMEOUT_MS;	// tiempo de espera

		if (select(conexionServidor+1, &rfds, NULL, NULL, &tv))
		{
			// Se espera un cliente que quiera conectarse
			conexionCliente = aceptarConexionCliente(conexionServidor, &clienteData);
			if (conexionCliente == -1)
			{
				printf ("No se puede abrir socket de cliente\n");
				return -1;
			}

			loop = -1;	// Para reiniciar el mensaje de Esperando conexión...

			switch (childpid = fork())
			{
			case -1:	// Error
				error(4, "No se puede crear el proceso hijo");
				break;

			case 0:		// somos proceso hijo
				if (childcount < MAX_CHILDS)
				{
					respuesta = atiendeCliente(conexionCliente, clienteData, cuentasClientes);	// atiendo una nueva conexion

					espera_semaforo(id_semaforo, NUM_SEMAFORO);
						cuentasClientes[respuesta.clientId-1] = respuesta.montoActualizado;		// actualizo el monto
					levanta_semaforo(id_semaforo, NUM_SEMAFORO);
				}
				else
					respuesta = demasiadosClientes(conexionCliente, clienteData);

				exit(respuesta.exitCode);	// código de salida

			default:					// Somos proceso padre
				childcount++;			// Acabamos de tener un hijo
				close(conexionCliente);	// Nuestro hijo se las apaña con el cliente que entró, para nosotros ya no existe.
				break;
			}

		}

		// Miramos si se ha cerrado algún hijo últimamente
		childpid = waitpid(0, &pidstatus, WNOHANG);
		if (childpid > 0)
		{
			childcount--;	// Se acaba de morir un hijo

			/* Muchas veces nos dará 0 si no se ha muerto ningún hijo, o -1 si no tenemos hijos
			con errno=10 (No child process). Así nos quitamos esos mensajes */

			if (WIFEXITED(pidstatus))
			{
				// Tal vez querremos mirar algo cuando se ha cerrado un hijo correctamente
				if (WEXITSTATUS(pidstatus) == 99)
				{
					printf("\nSe ha pedido el cierre del programa\n");
					activated = 0;
				}
			}
		}
	
		loop++;

	}

	close(conexionServidor);
	return 0;
}



// funcion que lee el archivo cuentas.dat y carga los datos en el vector de cuentas
/**
 * ******************************************************************** *
 * @brief Funcion que carga las cuentas									*
 * Leer el archivo cuentas.dat y carga los datos en el vector cuentas	*
 *  																	*
 * @param cuentas: puntero al vector donde se cargan los datos			*
 * 																		*
 * ******************************************************************** */
void cargoCuentas(int *cuentas)
{
	char	linea[LARGO_LINEA];
	FILE	*cuentasFile = NULL;
	int		clienteId;
	int		montoCuenta;

	cuentasFile = fopen(NOMBRE_ARCHIVO, "r");
	if (cuentasFile != NULL)
	{
		while (fgets(linea, LARGO_LINEA, cuentasFile) != NULL)
		{
			clienteId = atoi(strtok(linea, "|" ));
			montoCuenta = atoi(strtok(NULL, "|" ));
			cuentas[clienteId-1] = montoCuenta;
		}
		fclose(cuentasFile);
	}
	else
	{
		perror("Error al abrir el archivo de cuentas...\n");
	}
}
