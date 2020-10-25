all: llss

llss: main.c
	gcc -o runllss main.c client/client.c client/utils-client.c server/server.c server/utils-server.c utils/utils.c

clean: 
	rm -f runllss