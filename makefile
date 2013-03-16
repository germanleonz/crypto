all:    serv_acceso_seg	 acceso_rem_seg

acceso_rem_seg:		common.o acceso_rem_seg.o
					gcc common.o acceso_rem_seg.o -o acceso_rem_seg -l ssl

acceso_rem_seg.o:   acceso_rem_seg.o
					gcc -c acceso_rem_seg.c 

serv_acceso_seg:   	common.o serv_acceso_seg.o
					gcc common.o serv_acceso_seg.o -o serv_acceso_seg -l ssl -l pthread

serv_acceso_seg.o: 	serv_acceso_seg.c 
					gcc -c serv_acceso_seg.c

common.o:       	common.c common.h
					gcc -c common.c -l ssl 
