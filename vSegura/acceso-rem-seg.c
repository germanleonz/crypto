#include "common.h"

#define CERTFILE "client.pem"

/*
 *	Funcion: setup_client_ctx
 *	-------------------------
 *	Verifica los parametros introducidos por la linea de comandos
 * 
 *  argc:				cantidad de argumentos que se recibieron consola
 * 
 *  argv[]:				arreglo de argumentos pasados por la consola
 * 
 */
SSL_CTX* setup_client_ctx(void)
{
    SSL_CTX *ctx;

    ctx = SSL_CTX_new(SSLv23_method());
    if (SSL_CTX_use_certificate_chain_file(ctx, CERTFILE) != 1) 
        int_error("Error cargando certificado del archivo");
    if (SSL_CTX_use_PrivateKey_file(ctx, CERTFILE, SSL_FILETYPE_PEM) != 1) 
        int_error("Error cargando la clave privada del archivo");
    return ctx;
}

/*
 *	Funcion: comprobarParametros
 *	----------------------------
 *	Verifica los parametros introducidos por la linea de comandos
 * 
 *  argc:				cantidad de argumentos que se recibieron consola
 * 
 *  argv[]:				arreglo de argumentos pasados por la consola
 * 
 */
int comprobar_parametros(int argc, char* argv[], char* dir_servidor[], char * num_puerto[])
{
    switch(argc)
    {
        case 5:
            if ((strcmp(argv[1], "-s") == 0) && (strcmp(argv[3], "-p") == 0))
            {
                if ((atoi(argv[4]) > 1024) && (atoi(argv[4]) <= 65535))
                {
                    *dir_servidor = argv[2];
                    *num_puerto = argv[4];
                    return TRUE;
                }
            } else if ((strcmp(argv[1], "-p") == 0) && (strcmp(argv[3], "-s") == 0))
            {      
                 if ((atoi(argv[2]) > 1024) && (atoi(argv[2]) <= 65535))
                {
                    *dir_servidor = argv[4];
                    *num_puerto = argv[2];
                    return TRUE;
                }
            }
            break;
        default: //Para cualquier otro caso
            return FALSE;	
    }
}

/*
 *	Funcion: solicitar_login
 *	-----------------------
 *	Funcion que deja al cliente en un loop
 * 
 *  ssl	    Objeto SSL*
 * 
 */
int solicitar_login(SSL *ssl)
{
    //  El servidor envia el primer mensaje el cual es la solicitud
    //  del nombre de usuario
    int err;
    char aux[240];

    err = SSL_read(ssl, aux, sizeof(aux));
    fwrite(aux, 1, strlen(aux), stdout);

    char nombre_usuario[240];
    fgets(nombre_usuario, sizeof(nombre_usuario), stdin);
    err = SSL_write(ssl, nombre_usuario, strlen(nombre_usuario));

    //  Luego leemos la solicitud de contrasena
    memset(aux, 0, 240);
    err = SSL_read(ssl, aux, sizeof(aux));
    fwrite(aux, 1, strlen(aux), stdout);

    char clave[240];
    fgets(clave, sizeof(clave), stdin);
    err = SSL_write(ssl, clave, strlen(clave));

    //  Esperamos la respuesta del servidor una vez que este revise las credenciales
    memset(aux, 0, 240);
    err = SSL_read(ssl, aux, sizeof(aux));
    fwrite(aux, 1, strlen(aux), stdout);

    if (strcmp(aux, "Usuario autorizado\n") == 0) {
        //  Aqui comienza el loop de echo
        char aux2[240];
        while (TRUE) {
            fgets(aux2, sizeof(aux2), stdin);
            err = SSL_write(ssl, aux2, strlen(aux2));
            if (strcmp(aux2, "salir\n") == 0)
                return TRUE;
        }
    }
    return TRUE;
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
    char* dir_servidor;
    char* num_puerto;

    int parametros_ok = comprobar_parametros(argc, argv, &dir_servidor, &num_puerto);
    if (!parametros_ok) {
        int_error("Formato incorrecto de parametros");
    }

    //  Inicializacion de OpenSSL
    BIO     *conn;
    SSL     *ssl;
    SSL_CTX *ctx;
    
    init_OpenSSL();
    /*seed_prng();*/

    ctx = setup_client_ctx();
    
    //  Establecemos la conexion con el servidor
    char * direccion_puerto = malloc(snprintf(NULL, 0, "%s:%s", dir_servidor, num_puerto) + 1);
    sprintf(direccion_puerto, "%s:%s", dir_servidor, num_puerto);
    printf("Estableciendo conexion con: %s\n", direccion_puerto),

    conn = BIO_new_connect(direccion_puerto);
    if (!conn) 
        int_error("Error creando conexion BIO");
    
    if (BIO_do_connect(conn) <= 0) 
        int_error("Error conectandose a la maquina remota");

    if (!(ssl = SSL_new(ctx))) 
        int_error("Error creando un SSL context");
    SSL_set_bio(ssl, conn, conn);
    if (SSL_connect(ssl) <= 0) 
        int_error("Error conectando el objeto SSL");

    fprintf(stderr, "*** Conexion SSL abierta ***\n");
    if (solicitar_login(ssl))
        SSL_shutdown(ssl);
    else
        SSL_clear(ssl);
    fprintf(stderr, "*** Conexion SSL cerrada ***\n");

    //  Terminamos la conexion con el servidor
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}
