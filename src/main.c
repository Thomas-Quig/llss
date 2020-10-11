#include "client/client.h"
#include "server/server.h"
int main(int argc, char ** argv){
    if(argc >= 2 && strncmp(argv[1],"client",7)
    {
        // Run Client Main
        client_main(argc, argv);
    }
    else if(argc >= 2 && strncmp(argv[1],"server",7))
    {
        server_main(argc, argv);
    }

}