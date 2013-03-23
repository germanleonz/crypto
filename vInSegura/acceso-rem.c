#include <stdlib.h>
#include<stdio.h>	
#include<string.h>	
#include<sys/socket.h>	
#include<arpa/inet.h>	

/*
 *	Funcion: solicitar_login
 *	-----------------------
 *	Funcion que deja al cliente en un loop
 * 
 *  int	    soccket de conexion
 * 
 */

int solicitar_login(int sock)
{

	char message[240] , server_reply[240];

    //Recibe respuesta del Servidor
	if( recv(sock , server_reply , 2000 , 0) < 0){
		puts("falla al recibir mensaje.");
		return 1;
	}
    fwrite(server_reply, 1, strlen(server_reply), stdout);
    fgets(message, sizeof(message), stdin);
    //Envia datos
    if( send(sock , message , strlen(message) , 0) < 0){
   		puts("falla al enviar mensaje.");
   		return 1;
   	}
    memset (server_reply,'\0',240); 
	//Recibe respuesta del Servidor
	if( recv(sock , server_reply , 2000 , 0) < 0){
		puts("falla al recibir mensaje.");
		return 1;
	}
    fwrite(server_reply, 1, strlen(server_reply), stdout);
    fgets(message, sizeof(message), stdin);
    //Envia datos
    if( send(sock , message , strlen(message) , 0) < 0){
   		puts("falla al enviar mensaje.");
   		return 1;
   	}
 
    //  Esperamos la respuesta del servidor una vez que este revise las credenciales
    memset (server_reply,'\0',240);
	if( recv(sock , server_reply , 2000 , 0) < 0){
		puts("falla al recibir mensaje.");
		return 1;
	}
    fwrite(server_reply, 1, strlen(server_reply), stdout);
 
    if (strcmp(server_reply, "Usuario autorizado\n") == 0) {
        //  Aqui comienza el loop de echo
        while (1) {
            printf("Escribe mensaje (salir): ");
            fgets(message, sizeof(message), stdin);

            if( send(sock , message , strlen(message) , 0) < 0){
   		        puts("falla al enviar mensaje.");
   		        return 1;
   	        }
            memset (server_reply,'\0',240); 
            memset (message,'\0',240); 

	        if( recv(sock , server_reply , 240 , 0) < 0){
		        puts("falla al recibir mensaje.");
		        return 1;
	        }
            
            if (strcmp(server_reply, "salir\n") == 0)
                return 1;
            else  
                printf("Servidor: %s\n",server_reply);
        }
    }

    return 0;
    
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
int main(int argc , char *argv[]){

    if(argc != 5 || strcmp(argv[1],"-s")!=0 || strcmp(argv[4],"1024")<0){
        printf("\nUso: %s -s ip-servidor -p puerto-servidor \n",argv[0]);
        return 1;
    } 
system("clear");
printf("\n      #######################VERSION#######INSEGURA#########################\n");      
printf("      #  __________.__                                 .__    .___         #\n");
printf("      #  \\______   \\__| ____   _______  __ ____   ____ |__| __| _/____     #\n");
printf("      #   |    |  _/  |/ __ \\ /    \\  \\/ // __ \\ /    \\|  |/ __ |/  _ \\    #\n"); 
printf("      #   |    |   \\  \\  ___/|   |  \\   /\\  ___/|   |  \\  / /_/ (  <_> )   #\n");
printf("      #   |______  /__|\\___  >___|  /\\_/  \\___  >___|  /__\\____ |\\____/    #\n");
printf("      #          \\/        \\/     \\/          \\/     \\/        \\/          #\n"); 
printf("      #                                                                    #\n");
printf("      #######################VERSION#######INSEGURA#########################\n\n");


	int sock;
	struct sockaddr_in server;
	
	//Crea el socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1){
		printf("No se pudo crear el Socket.");
	}
//	puts("Socket creado.");
	
    if(strcmp(argv[2],"localhost")==0)
    	server.sin_addr.s_addr = inet_addr("127.0.0.1");
    else
    	server.sin_addr.s_addr = inet_addr(argv[2]);
	server.sin_family = AF_INET;
	server.sin_port = htons( atoi(argv[4]));

	//Conexion al servidor remoto
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("falla en la conexion. Error");
		return 1;
	}
	
	puts("Conectado... Espere...\n");
	
	solicitar_login(sock);
   
	close(sock);
	return 0;
}
