all : serv-acceso acceso-rem guardar-clave

acceso-rem.o:   	acceso-rem.c
					gcc -c acceso-rem.c 

acceso-rem:			acceso-rem.o
					gcc acceso-rem.o -o acceso-rem

serv-acceso.o: 		serv-acceso.c 
					gcc -c serv-acceso.c

serv-acceso:   		common.o serv-acceso.o
					gcc common.o serv-acceso.o -o serv-acceso -lssl -lpthread

common.o: 			common.c common.h
					gcc -c common.c -lssl 

guardar-clave: 		guardar-clave.o common.o
					gcc guardar-clave.o common.o -o guardar-clave -lssl

guardar-clave.o:	guardar-clave.c
					gcc -c guardar-clave.c

clean :
					rm *.o serv-acceso acceso-rem guardar-clave
