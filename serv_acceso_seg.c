#include "common.h"

#define CERTFILE "server.pem"

SSL_CTX* setup_server_ctx(void)
{
    SSL_CTX* ctx;

    ctx = SSL_CTX_new(SSLv23_method());
    if (SSL_CTX_use_certificate_chain_file(ctx, CERTFILE) != 1) 
        int_error("Error cargando el certificado del archivo");
    if (SSL_CTX_use_PrivateKey_file(ctx, CERTFILE, SSL_FILETYPE_PEM) != 1) 
        int_error("Error cargando clave privada del archivo");
    return ctx;
}

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
 */
int do_server_loop(SSL *ssl)
{
    int err, nread;
    char buf[80];

    do {
        for (nread = 0; nread < sizeof(buf); nread += err) {
            err = SSL_read(ssl, buf + nread, sizeof(buf) - nread);
            if (err <= 0) {
                break;
            }
        }
        fwrite(buf, 1, nread, stdout);
    } while (err > 0);
    return (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) ? 1 : 0;
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
    SSL *ssl = (SSL *) arg;

    pthread_detach(pthread_self());

    if (SSL_accept(ssl) <= 0)
        int_error("Error aceptando conexion SSL");
    fprintf(stderr, "Conexion SSL abierta\n");
    if (do_server_loop(ssl))
        SSL_shutdown(ssl);
    else 
        SSL_clear(ssl);
    fprintf(stderr, "Conexion SSL cerrada.\n");
    SSL_free(ssl);

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

    printf("Preparandose para recibir conexiones por el puerto %s ...\n", num_puerto);

    //  Inicializacion de OpenSSL
    BIO         *acc, *client;
    SSL         *ssl;
    SSL_CTX     *ctx;
    THREAD_TYPE tid;

    init_OpenSSL();
    /*seed_prng();*/

    ctx = setup_server_ctx();

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
        if (!(ssl = SSL_new(ctx)))
            int_error("Error creando contexto SSL");

        SSL_set_bio(ssl, client, client);
        THREAD_CREATE(tid, server_thread, ssl);
    }

    //  Dejamos de atender conexiones 
    SSL_CTX_free(ctx);
    BIO_free(acc);
    return 0;
}
