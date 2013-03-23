Archivo Leeme.
Autores:
    Arturo Murillo  08-10790
    German Leon     08-10611

En el archivo .tar.gz se encuentran dos carpetas, vInSegura y vSegura, 
cada una contiene los archivos respectivos para ejecutar el programa segun la 
version que desee.

Detalles de Archivos.
    Version InSegura:
        - acceso-rem.c:     Programa cliente que conecta al servidor mediante 
                            Sockets con TCP.
        - serv-acceso.c:    Programa servidor, crea hilo para cada cliente que 
                            se conecta, verifica sus credenciales, concediendo 
                            o negando acceso segun sea el caso.
        - common.c:         Define funciones necesarias para la obtencion del 
                            resumen criptografico, asi como la definicion de 
                            variables globales.
        - common.h:         Define las firmas de las funciones utilizadas en 
                            common.c
        - guardar-clave.c:  Programa guarda Nuevos usuarios y resumen 
                            criptografico de su respectiva clave en archivo 
                            "shadow"
        - makefile:         Archivo contenedor de las directrices necesarias 
                            para la compilacion exitosa del programa.

    Version Segura:
        - acceso-rem-seg.c:     Programa cliente que conecta al servidor mediante 
                                conexion segura OpenSSL con TLS.
        - serv-acceso-seg.c:    Programa servidor, crea hilo para cada cliente 
                                que se conecta, verifica sus credenciales, 
                                garantizando o negando acceso segun sea el caso.
        - common.c:             Define funciones necesarias para la obtencion 
                                del resumen criptografico, asi como la definicion 
                                de variables globales.
        - common.h:             Define las firmas de las funciones utilizadas 
                                en common.c
        - guardar-clave.c:      Programa guarda Nuevos usuarios yresumen 
                                criptografico de su respectiva clave en 
                                archivo "shadow"
        - Readme.md:            Archivo contenedor de las directrices necesarias 
                                para la creacion de los certificados.
        - client.pem:           Certificado x509 del cliente. 
        - server.pem:           Certificado x509 del servidor.
        - makefile:             Archivo contenedor de las directrices necesarias 
                                para la compilacion exitosa del programa.

El proceso de compilacion para ambas versiones se ve beneficiado con el archivo
makefile. Se generan 3 ejecutables: el servidor, el cliente, y el programa para 
guardar usuarios y clave.   

- Generacion de certificados:

Ver el archivo README.md de la carpeta vSegura

- Proceso de guardado de los nombre de usuarios y las claves

Modo de uso: ./guardar-clave -u <nombre_de_usuario> -c <clave>
Los usuarios y las claves son guardadas en un archivo "shadow". En dicho archivo 
se guarda el usuario junto a el resumen criptografico de su respectiva clave, 
cifradas mediante las funciones hash SHA-2. De esta manera cuando un usuario esta 
intentando autenticarse con el sistema se calcula el resumen criptografico de la
contrasena introducida y se compara con el resumen guardado en el archivo. 
