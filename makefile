all:    serv_acceso acceso_rem

acceso_rem:		common.o acceso_rem.o
				gcc common.o acceso_rem.o -o acceso_rem -l ssl

acceso_rem.o:   acceso_rem.o
				gcc -c acceso_rem.c 

serv_acceso:    common.o serv_acceso.o
				gcc common.o serv_acceso.o -o serv_acceso -l ssl -l pthread

serv_acceso.o:  serv_acceso.c 
				gcc -c serv_acceso.c

common.o:       common.c common.h
				gcc -c common.c -l ssl 
