#include "common.h"

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
