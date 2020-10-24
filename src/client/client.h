#include "utils-client.h"
#include "../utils/utils.h"

/**
*   Prints the client options
*   
**/
void send_user_content(char * ip,int port);
void send_content_loop(int sockfd, struct sockaddr_in servaddr);
void mac_change_loop();
void print_wizard_options();
int handle_client_options();
int client_main(int argc, char ** argv);