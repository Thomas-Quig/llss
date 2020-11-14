#include "client/client.h"
#include "server/server.h"
#include "utils/utils.h"
int main(int argc, char ** argv){
    if(argc == 1)
    {
        printf("usage: llss (cli,rcv,snd,svr) <args>\n");
        return 0;
    }
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
            printf("Running in client send mode...\n");
            return client_main(argc, argv, __CLIENT_SEND);
        }
        else if((!strncmp(argv[1],"rcv",4)) || || (!strncmp(argv[1],"receive",8)))
        {
            //Run the server given argc and argv
            printf("Running in client receive mode...\n");
            return client_main(argc, argv, __CLIENT_RECEIVE);
        }
        else if((!strncmp(argv[1],"svr",4)) || (!strncmp(argv[1],"server",7))))
        {
            //Run the server given argc and argv
            printf("Server is depracated, setting up client in explicit recieve mode.")
            return server_main(argc, argv);
        }
        else if(argc == 2 && (!strncmp(argv[1],"help",5) || !strncmp(argv[1],"-h",3) || !strncmp(argv[1],"--help",7)))
        {
            printf("usage: llss (client,server) <args>\n");
            return 0;
        }
    }

}