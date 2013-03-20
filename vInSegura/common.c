#include "common.h"

void handle_error(const char *file, int lineno, const char* msg)
{
    fprintf(stderr, "** %s: %i %s\n", file, lineno, msg);
    ERR_print_errors_fp(stderr);
    exit(-1);
}

void init_OpenSSL(void)
{
    if (!SSL_library_init()) {
        fprintf(stderr, " ** Inicializacion fallida de OpenSSL\n");
        exit(-1);
    }
    SSL_load_error_strings();
}

int calcular_SHA(char *texto, u_int8_t results[])
{
    SHA256_CTX ctx;
    int n;

    n = strlen(texto);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, (u_int8_t *) texto, n);
    SHA256_Final(results, &ctx);

    return TRUE;
}
