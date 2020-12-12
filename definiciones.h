#ifndef _DEFINICIONES_H
#define _DEFINICIONES_H


	#define	NOMBRE_ARCHIVO		"bd/cuentas.dat"	// path del archivo donde se guardan las cuentas
	#define	ARCHIVO_LOGS		"bd/logs.dat"		// path del archivo donde se gardaran todos los movimientos
	#define CANTIDAD_CLIENTES	10					// cantidad maxima de clientes soportada
	#define LARGO_LINEA 		100					// largo de la linea que se guarda en el archivo "cuentas.dat"


	typedef enum
	{
		EVT_SALIR,			// el cliente se desconecta
		EVT_CONSULTAR,		// consultar saldo
		EVT_EXTRAER,		// realizar una extraccion
		EVT_DEPOSITAR		// realizar un deposito
	}eventos;


	typedef struct transaccion
	{
		int		clientId;			// ID del cliente
		int		huboCambios;		// flag que indica que el monto fue actualizado
		int		montoActualizado;	// monto actualizado de su cuenta
		int		exitCode;			// codigo de salida
	} resultadoTransaccion;
	


#endif
