#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>

#include <pthread.h>
#define THREAD_CC
#define THREAD_TYPE                     pthread_t
#define THREAD_CREATE(tid, entry, arg)  pthread_create(&(tid), NULL, \
                                                       (entry), (arg))

#define int_error(msg)  handle_error(__FILE__, __LINE__, msg) 

#ifndef BOOLEANOS
#define BOOLEANOS
#define FALSE 0
#define TRUE 1
#endif

void handle_error(const char *file, int lineno, const char *msg);
    
void init_OpenSSL(void);

int calcular_SHA(char *texto, u_int8_t *results);
