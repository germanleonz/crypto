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
/*int comprobarParametros(int argc, char* argv[])*/
/*{*/
    /*switch(argc)*/
    /*{*/
        /*case 5:*/
            /*if ((strcmp(argv[1], "-n")) != 0)*/
                /*return FALSE;*/
            /*else*/
            /*{      */
                /*if ((atoi(argv[2]))<0 || (atoi(argv[2]))>64)*/
                    /*return FALSE;*/
                /*else*/
                /*{*/
                    /**num_trabaj = atoi(argv[2]);*/
                /*}*/
            /*}*/
            /*if((strcmp(argv[3], "-i")) != 0)   */
                /*return FALSE;*/
            /*else*/
            /*{      */
                /*if ((atoi(argv[4]))!=0 && (atoi(argv[4]))!=1)*/
                    /*return FALSE;*/
                /*else*/
                /*{*/
                    /**opcImpr = atoi(argv[4]);*/
                    /*return TRUE;*/
                /*}*/
            /*}					*/
            /*break;*/
        /*case 1: // No se recibe ningun parametro por consola*/
            /**num_trabaj = 8;*/
            /**opcImpr = 0;*/
            /*return TRUE;*/
            /*break;*/
        /*case 3:*/
            /*if (((strcmp(argv[1], "-n"))!= 0) && ((strcmp(argv[1],"-i")) != 0))*/
            /*{*/
                /*return FALSE;*/
            /*}*/
            /*if ((strcmp(argv[1], "-n")) == 0)*/
            /*{      */
                /*if ((atoi(argv[2])) < 1 || (atoi(argv[2])) > 64){*/
                    /*return FALSE;*/
                /*}*/
                /*else*/
                /*{*/
                    /**num_trabaj = atoi(argv[2]);*/
                    /**opcImpr = 0;*/
                    /*return TRUE;*/
                /*}*/
            /*}*/
            /*if ((strcmp(argv[1], "-i")) == 0)*/
            /*{      */
                /*if ((atoi(argv[2]))!=0 && (atoi(argv[2]))!=1){*/
                    /*return FALSE;*/
                /*}*/
                /*else*/
                /*{   */
                    /**num_trabaj = 8;*/
                    /**opcImpr = atoi(argv[2]);*/
                    /*return TRUE;*/
                /*}*/
            /*}*/
            /*break;*/
        /*default: //Para cualquier otro caso*/
            /*return FALSE;	*/
    /*}*/
/*}*/

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

int main(int argc, const char *argv[])
{
    char* server = argv[2];
    char* port = argv[4];

    BIO * conn;
    
    init_openSSL();
    
    conn = BIO_new_connect(server ":" port);
    if (!conn) 
        int_error("Error creating connection BIO");
    
    if (BIO_do_connect(conn) <= 0) {
        int_error("Error connecting to remote mcachine ");
    }

    fprintf(stderr, "Connection opened\n");
    do_client_loop(conn);
    fprintf(stderr, "Connection closed\n");

    BIO_free(conn);
    return 0;
}
