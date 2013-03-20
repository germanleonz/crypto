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

            printf("Clave en archivo:%s:\n", clave_aux);
            printf("Hash string: %s\n", hash_string);

            return strcmp(clave_aux, hash_string) == 0 ? TRUE : FALSE;
            break;
        }
    }

    return FALSE;
}


/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc){
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char nombre_usuario[240];
    char clave[240];
    memset (nombre_usuario,'\0',240);
    memset (clave,'\0',240);
    printf("Solicitando nombre de usuario...\n");
	//Send some messages to the client
	write(sock , "Nombre de usuario" , 20);
	//Receive a message from client
	if( (read_size = recv(sock , nombre_usuario , 2000 , 0)) > 0 ){
		//Send the message back to client
		write(sock , nombre_usuario , strlen(nombre_usuario));
	}

	if(read_size == 0){
		puts("Cliente desconectado.");
		fflush(stdout);
	}else if(read_size == -1){
		perror("recv failed");
	}

    printf("Nombre de usuario recibido. \n");

//        puts("Existe el Usuario.");

//        memset (client_message,'\0',strlen(client_message));
    //  Enviamos la solicitud de la contrasena
    printf("Solicitando clave...\n");
	write(sock , "Clave" , 5);
    memset (clave,'\0',240);
	//Receive a message from client
    if( (read_size = recv(sock , clave , 2000 , 0)) > 0 ){
	    //Send the message back to client
	    write(sock , clave , strlen(clave));
    }
	
	if(read_size == 0){
	    puts("Cliente desconectado.");
	    fflush(stdout);
    }else if(read_size == -1){
	    perror("recv failed");
    }

    printf("Clave recibida. \n");

//        memset (client_message,'\0',strlen(client_message));
// Verificando credenciales
    if (validar_credenciales(nombre_usuario, clave) == TRUE) {
        write(sock , "\nUsuario autorizado." , 23);
    } else {
        write(sock , "\nAcceso denegado." , 20);
    }

    sleep(1);
    write(sock , "salir" , 5);
	//Free the socket pointer
	free(socket_desc);
	
	return 0;
}

int main(int argc , char *argv[]){

    if(argc != 3 || strcmp(argv[2],"5000")<0){
        printf("\nUso: %s -p puerto\n",argv[0]);
        return 1;
    } 

	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1){
		printf("No se pudo crear el socket");
	}
	puts("Socket creado");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(  atoi(argv[2]) );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("fallo conexion. Error");
		return 1;
	}
	puts("conexion creada.");
	
	//Listen
	listen(socket_desc , 3);
	
	//Accept and incoming connection
	c = sizeof(struct sockaddr_in);
	
	
	//Accept and incoming connection
	puts("Esperando conexiones...");
	c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("\n\nConexion aceptada.");
		
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("No se pudo crear el hilo.");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
//		puts("Hilo asignado.");
	}
	
	if (client_sock < 0){
		perror("falla aceptando conexion.");
		return 1;
	}
	return 0;
}


