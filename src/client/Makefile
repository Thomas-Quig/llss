all: client

client: client.c
	gcc -o client -ggdb client.c utils-client.c 

server: server.c
	gcc -o server -ggdb server.c utils-server.c			

clean: 
	rm -f client server
