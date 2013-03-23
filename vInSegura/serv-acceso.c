#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <openssl/sha.h>
#include "common.h"

int validar_credenciales(char * nombre_usuario, char * clave)
{
    //  Abrir archivo con las claves
    int ok = 0;
    FILE * archivo;
    archivo = fopen("shadow", "r");
    if (archivo == NULL)
        perror("Error abriendo el archivo de claves");

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

            return strcmp(clave_aux, hash_string) == 0 ? TRUE : FALSE;
            break;
        }
    }

    return FALSE;
}


/*
 * Maneja la conexion para cada cliente.
 * */
void *atiende_conexion(void *socket_desc){
	
	int sock = *(int*)socket_desc;
	int read_size;
	char nombre_usuario[240];
    char clave[240];
    memset (nombre_usuario,'\0',240);
    memset (clave,'\0',240);
    //Solicita nombre de usuario al cliente.
    printf("Solicitando nombre de usuario...\n");
	write(sock , "Nombre de usuario: " , 20);
	//Recibe usuario.
	read_size = recv(sock , nombre_usuario , 240 , 0);
	if(read_size == 0){
		puts("Cliente desconectado.");
		fflush(stdout);
	}else if(read_size == -1){
		perror("recv failed");
	}
    char *pos = strchr(nombre_usuario, '\n');
    *pos = '\0';
    printf("Nombre de usuario recibido. \n");

    //  Enviamos la solicitud de la contrasena
    printf("Solicitando clave...\n");
	write(sock , "Clave: " , 7);
    memset (clave,'\0',240);
	//Recibe contrasena
    read_size = recv(sock , clave , 240 , 0);

	if(read_size == 0){
	    puts("Cliente desconectado.");
	    fflush(stdout);
    }else if(read_size == -1){
	    perror("recv failed");
    }
    pos = strchr(clave, '\n');
    *pos = '\0';
    printf("Clave recibida. \n");

    // Verificando credenciales
    if (validar_credenciales(nombre_usuario, clave) == TRUE) {
        write(sock , "Usuario autorizado\n" , 23);
        puts("Usuario autorizado. Reenviando mensajes al Usuario.");
        char aux2[240];
        int salir = 1;
        memset(aux2, '\0', 240);
        while(salir){
            read_size = recv(sock , aux2 , 240 , 0); 
            if(read_size == 0){
                puts("Cliente desconectado.");
                fflush(stdout);
                return 0;
            }else if(read_size == -1){
                perror("recv failed");
                return 0;
            }
            write(sock , aux2 , strlen(aux2));
            if(strcmp(aux2, "salir\n") == 0)
                salir = 0;
            memset(aux2, '\0', 240);

        }
        puts("Cliente desconectado.");       
    } else {
        write(sock , "Acceso denegado\n" , 20);
        puts("Acceso denegado");
    }

	//Libera socket
	free(socket_desc);
	
	return 0;
}

int main(int argc , char *argv[]){

    if(argc != 3 || strcmp(argv[2],"1024")<0){
        printf("\nUso: %s -p puerto\n",argv[0]);
        return 1;
    } 

	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;
	
	//Crea el socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1){
		printf("No se pudo crear el socket");
	}
	
	//Prepara la estrucura sockaddr_in 
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(  atoi(argv[2]) );
	
	//Conecta
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("fallo conexion. Error");
		return 1;
	}
	
	//Espera conexion
	listen(socket_desc , 3);
	
	c = sizeof(struct sockaddr_in);
	
	//Acepta la conexion
	printf("Servidor Iniciado. Esperando conexiones puerto %s...\n", argv[2]);
	c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("\n\nConexion aceptada.");
		
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		//crea el hilo que atendera la conexion
		if( pthread_create( &sniffer_thread , NULL ,  atiende_conexion , (void*) new_sock) < 0)
		{
			perror("No se pudo crear el hilo.");
			return 1;
		}
	}
	
	if (client_sock < 0){
		perror("falla aceptando conexion.");
		return 1;
	}
	return 0;
}
