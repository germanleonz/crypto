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
}

int main(int argc, const char *argv[])
{
    char* nombre_usuario;
    char * clave;

    int parametros_ok = comprobar_parametros(argc, argv, &nombre_usuario, &clave);
    if (!parametros_ok) {
        int_error("Formato incorrecto de parametros");
    } 

    printf("Nombre:%s:", nombre_usuario);
    printf("Clave:%s:", clave);

    FILE * archivo;
    archivo = fopen("shadow", "w+");
    if (archivo == NULL)
        int_error("Error abriendo el archivo de claves");

    char *pos = strchr(nombre_usuario, '\n');
    *pos = '\0';
    pos = strchr(clave, '\n');
    *pos = '\0';

    fprintf(archivo, "%s %s\n", nombre_usuario, clave);

    return TRUE;
}
