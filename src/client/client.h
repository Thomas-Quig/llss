#include "utils-client.h"
/**
*   Prints the client options
*   
**/
int custom_test_code(int argc, char ** argv);
void send_user_content(char * ip,int port);
void user_send_content_loop(connection * conn);
void mac_change_loop();
void print_wizard_options();
int handle_client_options();
void cleanup(uint8_t orig, char * ip);
int client_main(int argc, char ** argv);