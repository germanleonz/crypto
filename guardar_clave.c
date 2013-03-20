#include "common.h"

int comprobar_parametros(int argc, char* argv[], char* nombre_usuario[], char * clave[])
{
    switch(argc)
    {
        case 5:
            if ((strcmp(argv[1], "-u") == 0) && (strcmp(argv[3], "-c") == 0))
            {
                *nombre_usuario = argv[2];
                *clave = argv[4];
                return TRUE;
            } else if ((strcmp(argv[1], "-c") == 0) && (strcmp(argv[3], "-u") == 0))
            {      
                *clave = argv[2];
                *nombre_usuario = argv[4];
                return TRUE;
            }
            break;
        default: //Para cualquier otro caso
            return FALSE;	
    }
    return FALSE;
}

int main(int argc, const char *argv[])
{
    char* nombre_usuario;
    char * clave;

    int parametros_ok = comprobar_parametros(argc, argv, &nombre_usuario, &clave);
    if (!parametros_ok) {
        int_error("Formato incorrecto de parametros");
    } 

    FILE * archivo;
    archivo = fopen("shadow", "a+");
    if (archivo == NULL)
        int_error("Error abriendo el archivo de claves");

    //  Cifrar la clave con SHA
    u_int8_t hash[SHA256_DIGEST_LENGTH];
    calcular_SHA(clave, &hash);

    int n;
    fprintf(archivo, "%s ", nombre_usuario);
    for (n = 0; n < SHA256_DIGEST_LENGTH; n++) {
        fprintf(archivo, "%02x", hash[n]);
    }
    fprintf(archivo, "\n");

    fclose(archivo);
    return TRUE;
}
