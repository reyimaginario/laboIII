#include "funciones.h"


/**
 * ********************************************************************************************	*
 * @brief funcion que crea la key_t																*
 *  																							*
 * @param r_clave: clave utilizada para crear la key_t											*
 *  																							*
 * @return la key_t creada																		*
 * 																								*
 * ********************************************************************************************	*/
key_t creo_clave(int r_clave)
{
	key_t clave;
	clave = ftok ("/bin/ls", r_clave);	
	if (clave == (key_t)-1)
	{
		printf("No puedo conseguir clave para memoria compartida\n");
		exit(0);
	}
	return clave;
}


/**
 * ********************************************************************************************	*
 * @brief funcion que obtiene el ID de la memoria compartida									*
 *  																							*
 * @param size: tamanio de la memoria															*
 * @param clave: clave utilizda para crear la memoria											*
 *  																							*
 * @return ID de la memoria creada																*
 * 																								*
 * ********************************************************************************************	*/
int creo_id_memoria(int size, int clave)
{
	int id_memoria;
	id_memoria = shmget (creo_clave(clave), size, 0777 | IPC_CREAT);	// crea memoria x 50 byte, 
	if (id_memoria == -1)
	{
		printf("No consigo id para memoria compartida\n");
		exit (0);
	}
	return id_memoria;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que crea el espacio en memoria compartida										*
 *  																							*
 * @param size: tamanio de la memoria															*
 * @param r_id_memoria: ID de la memoria compartida												*
 * @param clave: clave utilizda para crear la memoria											*
 *  																							*
 * @return puntero (generico) a la memoria creada												*
 * 																								*
 * ********************************************************************************************	*/
void* creo_memoria(int size, int* r_id_memoria, int clave_base)
{
	void* ptr_memoria;
	int id_memoria;
	id_memoria = shmget(creo_clave(clave_base), size, 0777 | IPC_CREAT); 

	if (id_memoria == -1)
	{
		printf("No consigo id para memoria compartida\n");
		exit (0);
	}

	ptr_memoria = (void*) shmat(id_memoria, (char *)0, 0);

	if (ptr_memoria == NULL)
	{
		printf("No consigo memoria compartida\n");
		exit (0);
	}

	*r_id_memoria = id_memoria;
	return ptr_memoria;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que crea semaforos															*
 *  																							*
 * @param cuantos: cantidad de semaforos														*
 * @param clave_semaforo: clave utilizada para crear el semaforo								*
 *  																							*
 * @return ID del semaforo creado																*
 * 																								*
 * ********************************************************************************************	*/
int creo_semaforo(int cuantos, int clave_semaforo)
{
	key_t clave = creo_clave(clave_semaforo);
	int id_semaforo = semget(clave, cuantos, 0600|IPC_CREAT); 
	if(id_semaforo == -1)
	{
		printf("Error: no puedo crear semaforo\n");
		exit(0);
	}
	return id_semaforo;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion para iniciar el semaforo														*
 *  																							*
 * @param id_semaforo: ID del semaforo que queremos iniciar										*
 * @param cual: Numero del semaforo que queremos iniciar										*
 * @param valor: Valor que le queremos asignar al semaforo										*
 * 																								*
 * ********************************************************************************************	*/
void inicia_semaforo(int id_semaforo, int cual, int valor)
{
	semctl(id_semaforo, cual, SETVAL, valor);
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que levanta el semaforo														*
 *  																							*
 * @param id_semaforo: ID del semaforo que queremos iniciar										*
 * @param cual: Numero del semaforo que queremos iniciar										*
 * 																								*
 * ********************************************************************************************	*/
void levanta_semaforo(int id_semaforo, int cual)
{
	struct sembuf operacion;
	operacion.sem_num = cual;
	operacion.sem_op = 1; //incrementa el semaforo en 1
	operacion.sem_flg = 0;
	semop(id_semaforo,&operacion,1);
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que se queda esperando a que el semaforo este levantado (bloqueante)			*
 *  																							*
 * @param id_semaforo: ID del semaforo que queremos iniciar										*
 * @param cual: Numero del semaforo que queremos iniciar										*
 * 																								*
 * ********************************************************************************************	*/
void espera_semaforo(int id_semaforo, int cual)
{
	struct sembuf operacion;
	operacion.sem_num = cual;
	operacion.sem_op = -1; //decrementa el semaforo en 1
	operacion.sem_flg = 0;
	semop(id_semaforo,&operacion,1);

}


/**
 * ********************************************************************************************	*
 * @brief Funcion que crea una cola de mensajes													*
 *  																							*
 * @param clave: clave utilizada para crear la cola de mensajes									*
 *  																							*
 * @return ID de la cola de mensajes creada														*
 * 																								*
 * ********************************************************************************************	*/
int creo_id_cola_mensajes(int clave)
{
	int id_cola_mensajes = msgget (creo_clave(clave), 0600 | IPC_CREAT);
	if (id_cola_mensajes == -1)
	{
		printf("Error al obtener identificador para cola mensajes\n");
		exit (-1);
	}
	return id_cola_mensajes;
}


/**
 * ********************************************************************************************	*
 * @brief funcion que envia mensajes a la cola de mensajes										*
 *  																							*
 * @param id_cola_mensajes: ID de la cola de mensajes											*
 * @param rLongDest: destinatario																*
 * @param rIntRte: remitente																	*
 * @param rIntEvento: evento																	*
 * @param rpCharMsg: mensaje																	*
 * 																								*
 * ********************************************************************************************	*/
int enviar_mensaje(int id_cola_mensajes, long rLongDest, int rIntRte, int rIntEvento, char* rpCharMsg)
{
	mensaje msg;
	msg.long_dest 	= rLongDest;
	msg.int_rte    	= rIntRte;
	msg.int_evento 	= rIntEvento;
	strcpy(msg.char_mensaje, rpCharMsg);
	return msgsnd (id_cola_mensajes, (struct msgbuf *)&msg, sizeof(msg.int_rte)+sizeof(msg.int_evento)+sizeof(msg.char_mensaje), IPC_NOWAIT);
}


/**
 * ********************************************************************************************	*
 * @brief funcion que recibe mensajes a la cola de mensajes										*
 *  																							*
 * @param id_cola_mensajes: ID de la cola de mensajes											*
 * @param rLongDest: destinatario																*
 * @param rMsg: mensaje																			*
 * 																								*
 * ********************************************************************************************	*/
int recibir_mensaje(int id_cola_mensajes, long rLongDest, mensaje* rMsg)
{
	mensaje msg;
	int res;
	res = msgrcv (id_cola_mensajes, (struct msgbuf *)&msg, sizeof(msg.int_rte)+sizeof(msg.int_evento)+sizeof(msg.char_mensaje), rLongDest, 0);
	
	rMsg->long_dest		= msg.long_dest;
	rMsg->int_rte		= msg.int_rte;
	rMsg->int_evento	= msg.int_evento;
	strcpy(rMsg->char_mensaje, msg.char_mensaje);
	return res;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que borra la cola de mensajes													*
 *  																							*
 * @param id_cola_mensajes: ID de la cola de mensajes											*
 * 																								*
 * ********************************************************************************************	*/
int borrar_mensajes(int id_cola_mensajes)
{
	mensaje msg;
	int res;
	do
	{
		res = msgrcv (id_cola_mensajes, (struct msgbuf *)&msg, sizeof(msg.int_rte)+sizeof(msg.int_evento)+sizeof(msg.char_mensaje), 0, IPC_NOWAIT);
	}while(res>0);
	return res;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que obrtiene valores aleatorios dentro de un rango de valores					*
 *  																							*
 * @param desde: valor inicial del rango														*
 * @param hasta: valor final del rango															*
 *  																							*
 * @return valor aleatorio obtenido dentro del rango											*
 * 																								*
 * ********************************************************************************************	*/
int obtenerValorRandomEntre(int desde, int hasta)
{
	int num;
	int hastaRand = (hasta - desde + 1);
	num = (rand()%hastaRand) + desde;
	return num;
}


/**
 * ********************************************************************************************	*
 * @brief Funcion que obtiene una lista de numeros aleatorios sin repetir						*
 *  																							*
 * @param desde: valor inicial del rango														*
 * @param hasta: valor final del rango															*
 * @param cantidad: cantidad de valores a devolver												*
 *  																							*
 * @return puntero (int) al vector con la lista de valores aleatorios							*
 * 																								*
 * ********************************************************************************************	*/
int* generarRandom(int desde, int hasta, int cantidad)
{
	/* * * El 'desde' no es incluido en la seleccion del numero aleatorio. Ejemplo: para obtener el valor '0' debo arrancar desde el valor '-1' * * */ 

	int n,num,i,j,hastaRand,temp;
	int* vector = (int*) malloc(cantidad*sizeof(int));
	
	desde--;
	if((hasta-desde) < cantidad)
	{		
		printf("deben haber mas numeros que cantidades, %d es menor que %d\n",(hasta-desde),cantidad);
		free(vector);
		exit (0);
		    
	}
    
	hastaRand = (hasta-desde)/cantidad ;
   
	temp = desde;
	
	for (n=0 ; n < cantidad ; n++)
	{	
		num = (rand()%(hastaRand)) + 1;
		vector[n] = temp + num;
		//printf("el numero es %d: %d =%d \n", vector[n],hastaRand,num);	
     		temp = temp + hastaRand;
	}

	//printf("Reordeno---------\n");	
	for (i=cantidad-1 ; i > 0 ; i--)
	{
		num = rand()%(i);
	 	j = vector[i];
		vector[i] = vector[num];
		vector[num] = j;
	}

	return vector;
}

