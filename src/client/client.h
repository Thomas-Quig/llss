#include "utils-client.h"

#define __SEND_MESSAGE 1
#define __SEND_FILE 2

int custom_test_code(int argc, char ** argv);
void chat(char * ip,int port);
int chat_loop(connection * conn);

void send_content(char * ip, int port, char * path, int mode);
size_t send_loop(connection * conn, char * content, size_t content_size);

void recv_content(char * ip, int port);
int recv_loop(connection * conn);

void mac_change_loop();
void print_wizard_options();
int handle_client_options();
void cleanup(char * orig, char * ip);
int client_main(int argc, char ** argv, int mode);