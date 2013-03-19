all:    serv_acceso_seg	 acceso_rem_seg guardar_clave

acceso_rem_seg:		common.o acceso_rem_seg.o
					gcc common.o acceso_rem_seg.o -o acceso_rem_seg -lssl

acceso_rem_seg.o:   acceso_rem_seg.o
					gcc -c acceso_rem_seg.c 

serv_acceso_seg:   	common.o serv_acceso_seg.o
					gcc common.o serv_acceso_seg.o -o serv_acceso_seg -lssl -lpthread

serv_acceso_seg.o: 	serv_acceso_seg.c 
					gcc -c serv_acceso_seg.c

guardar_clave:		guardar_clave.o
					gcc guardar_clave.o common.o -o guardar_clave -lssl

guardar_clave.o:	guardar_clave.c
					gcc -c guardar_clave.c

common.o:       	common.c common.h
					gcc -c common.c -lssl 

clean:				
					/bin/rm *.o serv_acceso_seg acceso_rem_seg guardar_clave
