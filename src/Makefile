all: llss

llss: main.c
	gcc -o runllss -lssl -lcrypto -lexplain main.c client/client.c client/utils-client.c server/server.c server/utils-server.c utils/utils.c

debug: main.c
	gcc -o debugllss -lssl -lcrypto -lexplain -g main.c client/client.c client/utils-client.c server/server.c server/utils-server.c utils/utils.c

clean: 
	rm -f runllss
	rm -f debugllss
