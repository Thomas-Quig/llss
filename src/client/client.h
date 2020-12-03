#include "utils-client.h"

#define __MAX_BUFFER_SIZE 16384
#define __SEND_MESSAGE 1
#define __SEND_FILE 2

typedef struct _args {
    int _mode;
    char _target_ip[16]; 
    int _port; 
    char _out_path[128];
    char _conf_path[128];
    char _log_path[128];
    char * _data;
} args;

int custom_test_code(int argc, char ** argv);
void chat(char * ip,int port);
int chat_loop(connection * conn);

void send_content(char * ip, int port, char * path, int mode);
size_t send_loop(connection * conn, char * content, size_t content_size);

void recv_content(char * ip, int port);
int recv_loop(connection * conn);

void sig_handler(int signo);
void mac_change_loop();
void configure(char * path);
void print_wizard_options();
void print_help();
void wizard();
int handle_client_options();
void cleanup(char * orig, char * ip);
int client_main(int argc, char ** argv);
void parse_args(args * a, int argc, char ** argv);