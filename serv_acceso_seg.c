#include "common.h"
#include <openssl/sha.h>

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

int validar_credenciales(char * nombre_usuario, char * clave)
{
    //  Abrir archivo con las claves
    int ok = 0;
    FILE * archivo;
    archivo = fopen("shadow", "r");
    if (archivo == NULL)
        int_error("Error abriendo el archivo de claves");

    //  Buscar la clave que corresponde a nombre_usuario
    char linea[BUFSIZ];
    char nombre_aux[512];
    char clave_aux[512];

    while (fgets(linea, sizeof(linea), archivo)) {
        sscanf(linea, "%s %s", nombre_aux, clave_aux);
        if (strcmp(nombre_aux, nombre_usuario) == 0) {
            //  Encontramos el usuario verificamos su clave
            u_int8_t hash[SHA256_DIGEST_LENGTH];
            calcular_SHA(clave, hash);

            char* hash_string;
            hash_string = (char *) malloc(SHA256_DIGEST_LENGTH * sizeof(char) * 8); 

            int i;
            for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                sprintf(hash_string + (2*i), "%02x", hash[i]);
            }

            printf("Clave en archivo:%s:\n", clave_aux);
            printf("Hash string: %s\n", hash_string);

            return strcmp(clave_aux, hash_string) == 0 ? TRUE : FALSE;
            break;
        }
    }

    return FALSE;
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
    //  Enviamos la solicitud del nombre de usuario
    printf("Solicitando nombre de usuario...\n");
    const char prompt_nombre[] = "Nombre de usuario: ";
    SSL_write(ssl, prompt_nombre, strlen(prompt_nombre));

    char nombre_usuario[240];
    SSL_read(ssl, nombre_usuario, sizeof(nombre_usuario));
    char *pos = strchr(nombre_usuario, '\n');
    *pos = '\0';
    printf("Nombre de usuario recibido.\n");

    //  Enviamos la solicitud de la contrasena
    printf("Solicitando clave ...\n");
    const char prompt_clave[] = "Clave: ";
    SSL_write(ssl, prompt_clave, strlen(prompt_clave));

    char clave[240];
    SSL_read(ssl, clave, sizeof(clave));
    pos = strchr(clave, '\n');
    *pos = '\0';
    printf("Clave recibida.\n");

    // Verificando credenciales
    if (validar_credenciales(nombre_usuario, clave) == TRUE) {
        const char acceso_autorizado[] = "Usuario autorizado\n";
        SSL_write(ssl, acceso_autorizado, strlen(acceso_autorizado));
    } else {
        const char acceso_negado[] = "Acceso negado\n";
        SSL_write(ssl, acceso_negado, strlen(acceso_negado));
    }

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

    fprintf(stderr, "*** Conexion SSL abierta ***\n");
    if (do_server_loop(ssl))
        SSL_shutdown(ssl);
    else 
        SSL_clear(ssl);
    fprintf(stderr, "*** Conexion SSL cerrada ***\n");

    //  Terminamos la conexion con el cliente
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
            int_error("Error aceptando conexion");

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
