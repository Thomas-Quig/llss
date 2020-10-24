#include "client/client.h"
#include "server/server.h"
#include "utils/utils.h"
int main(int argc, char ** argv){
    if(argc == 1)
    {
        printf("usage: llss (client,server) <args>\n");
        return 0;
    }
    if(argc >= 2 && (!strncmp(argv[1],"client",7) || argc == 1))
    {
        //Run the client given argc and argv
        return client_main(argc, argv);
    }
    else if(argc >= 2 && (!strncmp(argv[1],"server",7)))
    {
        //Run the server given argc and argv
        return server_main(argc, argv);
    }
    else if(argc == 2 && (!strncmp(argv[1],"help",5) || !strncmp(argv[1],"-h",3) || !strncmp(argv[1],"--help",7)))
    {
        printf("usage: llss (client,server) <args>\n");
        return 0;
    }

}