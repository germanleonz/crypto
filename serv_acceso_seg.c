#include "common.h"

/*
 *	Funcion: comprobar_parametros
 *	-----------------------------
 *	Hace la verificacion de los parametros introducidos por la linea de comandos
 * 
 *  argc:				cantidad de argumentos que se recibieron consola
 * 
 *  argv[]:				arreglo que contiene los argumentos que se pasaron por
 *						consola
 * 
 */
int comprobar_parametros(int argc, char* argv[], char* num_puerto[])
{
	switch(argc)
        {
		case 3: 
            //Se recibe el numero del puerto donde correra el servidor
			if ((strcmp(argv[1], "-p")) != 0)
				return FALSE;
			else
			{
				if ((atoi(argv[2]) < 1024) || (atoi(argv[2]) > 65535))
					return FALSE;
				else
					*num_puerto = argv[2];
			}
            break;
		default: // En cualquier otro caso
			return FALSE;	
	}
    return TRUE;
}

/*
 *	Funcion: do_server_loop
 *	-----------------------------
 *	Hace la verificacion de los parametros introducidos por la linea de comandos
 * 
 *  argc:				cantidad de argumentos que se recibieron consola
 * 
 *  argv[]:				arreglo que contiene los argumentos que se pasaron por
 *						consola
 * 
 */
void do_server_loop(BIO * conn)
{
    int err, nread;
    char buf[80];

    do {
        for (nread = 0; nread < sizeof(buf); nread += err) {
            err = BIO_read(conn, buf + nread, sizeof(buf) - nread);
            if (err <= 0) {
                break;
            }
        }
        fwrite(buf, 1, nread, stdout);
    } while (err > 0);
}

/*
 *	Funcion: THREAD_CC
 *	------------------
 *	Crea un nuevo hilo para atender una nueva solicitud 
 * 
 *  argc:				cantidad de argumentos que se recibieron consola
 * 
 *  argv[]:				arreglo que contiene los argumentos que se pasaron por
 *						consola
 * 
 */
void THREAD_CC server_thread(void *arg)
{
    BIO *client = (BIO *)arg;

    pthread_detach(pthread_self());
    fprintf(stderr, "Connection opened\n");
    do_server_loop(client);
    fprintf(stderr, "Connection closed.\n");

    BIO_free(client);
    ERR_remove_state(0);
}

/*
 *	Funcion: main
 *	-------------
 *	Flujo principal de ejecucion del programa cliente
 * 
 *  argc:				cantidad de argumentos que se recibieron consola
 * 
 *  argv[]:				arreglo de argumentos pasados por la consola
 * 
 */
int main(int argc, const char *argv[])
{
    char* num_puerto;

    int parametros_ok = comprobar_parametros(argc, argv, &num_puerto);
    if (!parametros_ok) {
        int_error("Formato incorrecto de parametros");
    } 

    printf("Escuchando conexiones por el puerto %s ...\n", num_puerto);

    BIO *acc, *client;
    THREAD_TYPE tid;

    //  Inicializacion de OpenSSL
    init_OpenSSL();

    //  Escuchamos por el numero de puerto especificado por el usuario
    acc = BIO_new_accept(num_puerto);
    if (!acc) 
        int_error("Error creating server socket.");
    if (BIO_do_accept(acc) <= 0) 
        int_error("Error binding server socket");

    //  Esto es lo que se hace una vez que estamos esperando conexiones
    for (;;) {
        if (BIO_do_accept(acc) <= 0) 
            int_error("Error accepting connection");

        client = BIO_pop(acc);
        THREAD_CREATE(tid, server_thread, client);
    }

    //  Dejamos de atender conexiones 
    BIO_free(acc);
    return 0;
}
