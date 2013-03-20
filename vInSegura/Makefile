all : serv_acceso acceso-rem guardar_clave

CPPFLAGS = -g -I.

acceso_rem.o:   acceso_rem.c
				gcc -c acceso-rem.c 

acceso_rem:		acceso_rem.o
				gcc acceso_rem.o -o acceso_rem

serv_acceso.o: 	serv_acceso.c 
				gcc -c serv_acceso.c

serv_acceso:   	common.o serv_acceso.o
				gcc common.o serv_acceso.o -o serv_acceso -lssl -lpthread


common.o: common.c common.h
	gcc -c common.c -lssl 

guardar_clave: guardar_clave.o common.o
	gcc guardar_clave.o common.o -o guardar_clave -lssl

guardar_clave.o:	guardar_clave.c
	gcc -c guardar_clave.c

clean :
	rm *.o serv_acceso acceso-rem guardar_clave
