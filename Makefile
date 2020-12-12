CC 		= g++ 

all : server client

server : server.c socketServidor.h serverFunciones.h funciones.h socketServidor.o serverFunciones.o funciones.o
	cc -o server server.c socketServidor.o serverFunciones.o funciones.o

client : client.c socketCliente.h definiciones.h socketCliente.o
	cc -o client client.c socketCliente.o

socketServidor.o : socketServidor.c socketServidor.h
	cc -c socketServidor.c

socketCliente.o : socketCliente.c socketCliente.h
	cc -c socketCliente.c

serverFunciones.o : serverFunciones.c serverFunciones.h
	cc -c serverFunciones.c

funciones.o : funciones.c funciones.h
	cc -c funciones.c
	
clean:
	rm -rf *o server client *~