all:    serv-acceso-seg	 acceso-rem-seg guardar-clave

acceso-rem-seg:		common.o acceso-rem-seg.o
					gcc common.o acceso-rem-seg.o -o acceso-rem-seg -lssl

acceso-rem-seg.o:   acceso-rem-seg.o
					gcc -c acceso-rem-seg.c 

serv-acceso-seg:   	common.o serv-acceso-seg.o
					gcc common.o serv-acceso-seg.o -o serv-acceso-seg -lssl -lpthread

serv-acceso-seg.o: 	serv-acceso-seg.c 
					gcc -c serv-acceso-seg.c

guardar-clave:		guardar-clave.o
					gcc guardar-clave.o common.o -o guardar-clave -lssl

guardar-clave.o:	guardar-clave.c
					gcc -c guardar-clave.c

common.o:       	common.c common.h
					gcc -c common.c -lssl 

clean:				
					/bin/rm *.o serv-acceso-seg acceso-rem-seg guardar-clave
