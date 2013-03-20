/*
	C ECHO client example using sockets
*/
#include<stdio.h>	//printf
#include<string.h>	//strlen
#include<sys/socket.h>	//socket
#include<arpa/inet.h>	//inet_addr

int main(int argc , char *argv[]){

    if(argc != 5 || strcmp(argv[1],"-s")!=0 || strcmp(argv[4],"5000")<0){
        printf("\nUso: %s -s ip-servidor -p puerto-servidor \n",argv[0]);
        return 1;
    } 
      
	int sock;
	struct sockaddr_in server;
	char message[1000] , server_reply[2000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1){
		printf("No se pudo crear el Socket.");
	}
	puts("Socket creado.");
	
    if(strcmp(argv[2],"localhost")==0)
    	server.sin_addr.s_addr = inet_addr("127.0.0.1");
    else
    	server.sin_addr.s_addr = inet_addr(argv[2]);
	server.sin_family = AF_INET;
	server.sin_port = htons( atoi(argv[4]));

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("falla en la conexion. Error");
		return 1;
	}
	
	puts("Conectado... Espere...\n");
	
	//keep communicating with server
    int salir = 1;
	while(salir){

		//Receive a reply from the server
		if( recv(sock , server_reply , 2000 , 0) < 0){
			puts("falla al recibir mensaje.");
			break;
		}
        
        if(strcmp(server_reply,"salir")==0)
            salir = 0;
        else
		    printf("server: %s \n",server_reply);

        if(strcmp(server_reply,"Nombre de usuario")==0 || strcmp(server_reply,"Clave")==0){
		    scanf("%s" , message);
		    printf("\n");
		    //Send some data
		    if( send(sock , message , strlen(message) , 0) < 0){
    			puts("falla al enviar mensaje.");
    			return 1;
    		}
        }
		
        memset (server_reply,'\0',strlen(server_reply));
	}
	
	close(sock);
	return 0;
}
