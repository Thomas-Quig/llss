all: llss

llss: main.c
	gcc -o runllss main.c client/client.c client/utils-client.c utils/utils.c -lssl -lcrypto -lexplain

debug: main.c
	gcc -o debugllss -g main.c client/client.c client/utils-client.c utils/utils.c -lssl -lcrypto -lexplain

clean: 
	rm -f runllss
	rm -f debugllss
