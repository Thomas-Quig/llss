#include "client/client.h"
#include "utils/utils.h"
int main(int argc, char ** argv){
    if(argc == 1)
    {
        printf("usage: llss (cli,rcv,snd,cht) <ip> [args] [msg]\nPlease use help or -h for options.\n");
        return 0;
    }
    client_main(argc,argv);
    /*
    if(argc >= 2)
    {
        if((!strncmp(argv[1],"cli",4)) || (!strncmp(argv[1],"client",7)))
        {
            //Run the client given argc and argv
            return client_main(argc, argv, __CLIENT_MAIN);
        }
        else if((!strncmp(argv[1],"snd",4)) || (!strncmp(argv[1],"send",5)))
        {
            //Run the server given argc and argv
            return client_main(argc, argv, __CLIENT_SEND);
        }
        else if((!strncmp(argv[1],"rcv",4)) || (!strncmp(argv[1],"receive",8)))
        {
            //Run the server given argc and argv
            return client_main(argc, argv, __CLIENT_RECV);
        }
        else if((!strncmp(argv[1],"cht",4)) || (!strncmp(argv[1],"chat",4)))
        {
            //Run the server given argc and argv
            return client_main(argc, argv, __CLIENT_CHAT);
        }
        else if(argc == 2 && (!strncmp(argv[1],"help",5) || !strncmp(argv[1],"-h",3) || !strncmp(argv[1],"--help",7)))
        {
            printf("usage: llss (cli,snd,rcv,cht) <ip> <port> [args]\n");
            return 0;
        }
        
    }*/
}