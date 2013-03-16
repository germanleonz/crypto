#include "common.h"

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
 *	Funcion: do_client_loop
 *	-----------------------
 *	Funcion que deja al cliente en un loop
 * 
 *  conn:	Objeto BIO*
 * 
 */
void do_client_loop(BIO *conn)
{
    int err, nwritten;
    char buf[80];

    for (;;){
        if (!fgets(buf, sizeof(buf), stdin)) 
            break;

        for (nwritten = 0; nwritten < sizeof(buf); nwritten += err) {
            err = BIO_write(conn, buf + nwritten, strlen(buf) - nwritten);
            if (err <= 0) {
                return;
            }
        }
    }
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

    BIO * conn;
    
    //  Inicializacion de OpenSSL
    init_OpenSSL();
    
    //  Establecemos la conexion con el servidor
    char * direccion_puerto = malloc(snprintf(NULL, 0, "%s:%s", dir_servidor, num_puerto) + 1);
    sprintf(direccion_puerto, "%s:%s", dir_servidor, num_puerto);
    printf("Estableciendo conexion con: %s\n", direccion_puerto),

    conn = BIO_new_connect(direccion_puerto);
    if (!conn) 
        int_error("Error creating connection BIO");
    
    if (BIO_do_connect(conn) <= 0) {
        int_error("Error connecting to remote machine ");
    }

    fprintf(stderr, "Connection opened\n");
    do_client_loop(conn);
    fprintf(stderr, "Connection closed\n");

    BIO_free(conn);
    return 0;
}
