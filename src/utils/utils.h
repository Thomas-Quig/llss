#include "includes.h"
#pragma once

#define __CLIENT_MAIN 0
#define __CLIENT_SEND 1
#define __CLIENT_RECV 2

#define __DEFAULT_FRAG_SIZE 1024
#define __ADVANCE_SYNC 0
#define __ADVANCE_ASYNC 1

#define __VERBOSE_STD 1
#define __VERBOSE_FUNC 2



#define max(a,b) \
    ({__typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? a : _b;})

#define min(a,b) \
    ({__typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? a : _b;})



/**
 *  A UDP "Connection" consists of the IP (for easy printing), the port (for easy printing), the socket fd (so it can be used), 
 * and the sockaddr_in for actually sending stuff. Its not actually a connection as this is UDP, but I am calling it a connection because it has all the
 * makings of one without it actually being connected.
 * @param ip the ip that you are connecting to, space for full "AAA.BBB.CCC.DDD\x00"
 * @param port the port you want to send UDP packets to.
 * @param fd the file descriptor connected to the socket
 * @param s_addr the socket address that you be sending packets to.
 * @param secret 256 bit shared secret key created by the diffie hellman process when establishing a "connection" between two hosts.
 **/
typedef struct _connection{
    int mode;
    char ip[16];
    int port;
    int fd;
    struct sockaddr_in s_addr;
    socklen_t s_len;
    char secret[32];
} connection;


/**
 * Config struct holds all the global configuration options.
 * This config file is passed into every major function, 
 * where it will sit to be used should it be neccesary
 * 
 * @param _VERBOSE should debug statements be printed, this can be enabled by using -v or --verbose. False by default
 * @param _FUNCLIST print off the function and arguments for each function as it is called. False by default
 * @param _SHUFFLE should the host shuffle it's MAC addresses? (Mostly debug). Turning _SHUFFLE off will allow it to run in any network configuration (with dhcp).
 * @param _ENCRYPT determines if the message should be encrypted using AES, true by default, turning off increases program speed.
 * @param _CLEANUP cleans up the arp table upon completion of the session. True by default
 * @param _LOG_SYS if true, any debug based statements will be sent to a log file instead
 * @param _CHECK_FILE if true, this will check to see if a file exists by the name of [message]
 * 
 * 
 * @param _OUTPUT_FD fd of where the received output goes.
 * @param _DB_OUTPUT_FD fd of where debug output goes.
 * @param _CSTMSEED a custom seed for MAC address shuffling, suggested if using unencrypted. Default is -1 which signifies no custom seed has been set.
 * @param _SEND_DELAY The send delay in microseconds
 * @param _FRAG_SIZE the fragmentation size, default 1024
 * @param _IFACE the interface to run this on
 **/
typedef struct _config{
    int  _VERBOSE;            //False by default
    int  _FUNCLIST;         //False by default
    int  _SHUFFLE;          //True by default
    int  _ENCRYPT;          //True by default
    int  _CLEANUP;          //True by default
    int  _LOG_SYS;          //False by default
    int  _CHECK_FILE;        //False by default
    
    int  _FRAG_SIZE;        //Sending fragmentation size
    int  _CSTMSEED;         //-1 By default
    int  _SEND_DELAY;       //10000 by default
    int  _ADVANCE_MODE;    //1 By default

    int  _OUTPUT_FD;        //STDOUT_FILENO By default
    int  _DB_OUTPUT_FD;     //STDOUT_FILENO by default
    
    char _IFACE[6];         //"wlan0" by default
} config;

extern config _global_conf;

ssize_t s_send(connection * conn, char * data, size_t size);
ssize_t s_recv(connection * conn, char * data, size_t size);
/**
 * Establishes a "connection" with the given address and port, passes ip and port into the connection if it is needed.
 * Returns on the heap, so it needs to be freed.
 * 
 * @param addr the ip address you are connecting to
 * @param port the port you are connecting to
 * @param mode 1 for sender (send pub key, then recieve pub key)
 *
 **/
connection * establish_connection(char * addr, int port, int mode);


char * estab_shared_secret(connection * conn,int mode);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);
/**
*	Gets the MAC Address 
*	It does the thing, creates 12B of space on the heap
**/
unsigned char * get_mac(char * iface);

/**
*	Formats the mac address
*	Creates 18B of space on the heap
**/
char * format_mac(unsigned char * mac);

//Parses first character of a string and quantifies it as a bool, -1 on error (not true, 1, or yes)
int boolify(char * input);

ssize_t save_config(char * fname);

void printConnection(connection * conn);

void handleErrors();
int _sys_log(int type, const char * format,...);