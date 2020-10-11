all: client

client: client/client.c
	gcc -o runclient client/client.c

server: server/server.c
	gcc -o runserver server/server.c	

debugc: client/client.c
	gcc -o clientdebug -ggdb client/client.c
	
debugs: server/server.c
	gcc -o serverdebug -ggdb server/server.c

clean: 
	rm -f runclient runserver
