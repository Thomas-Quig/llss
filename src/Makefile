all: llss

llss: main.c
	gcc -o runllss -lssl -lcrypto main.c client/client.c client/utils-client.c server/server.c server/utils-server.c utils/utils.c

debug: main.c
	gcc -o debugllss -g main.c client/client.c client/utils-client.c server/server.c server/utils-server.c utils/utils.c

clean: 
	rm -f runllss