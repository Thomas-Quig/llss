all: llss

llss: main.c
	gcc -o llss.o main.c client/client.c client/utils-client.c server/server.c server/utils-server.c utils/utils.c

clean: 
	rm -f llss.o