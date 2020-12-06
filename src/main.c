#include "client/client.h"
#include "utils/utils.h"
int main(int argc, char ** argv){
    if(argc == 1){
        printf("usage: llss (cli,rcv,snd,cht) <ip> [args] [msg]\nPlease use help or -h for options.\n");
        return 0;
    }
    client_main(argc,argv);
}