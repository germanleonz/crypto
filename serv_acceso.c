#include "common.h"
    
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

int main(int argc, const char *argv[])
{
    char* port = argv[2];
    printf("%s\n", port);

    BIO *acc, *client;
    THREAD_TYPE tid;

    init_openSSL();

    acc = BIO_new_accept(port);
    if (!acc) 
        int_error("Error creating server socket.");
    if (BIO_do_accept(acc) <= 0) 
        int_error("Error binding server socket");

    for (;;) {
        if (BIO_do_accept(acc) <= 0) 
            int_error("Error accepting connection");

        client = BIO_pop(acc);
        THREAD_CREATE(tid, server_thread, client);
    }

    BIO_free(acc);
    return 0;
}
