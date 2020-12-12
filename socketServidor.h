#ifndef _SOCKET_SERVIDOR_H
#define _SOCKET_SERVIDOR_H

int abrirSocket(char *);
int aceptarConexionCliente(int, struct sockaddr_in *);

#endif
