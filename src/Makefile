all: llss

llss: main.c
	gcc -o runllss main.c client/client.c server/server.c utils/utils.c

clean: 
	rm -f runllss