#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <assert.h>
#include <inttypes.h> 
#include <ctype.h>
#include <netinet/in.h>       // IPPROTO_RAW
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <bits/ioctls.h>      // defines values for argument "request" of ioctl.
#include <net/if.h>           // struct ifreq
#include <linux/if_ether.h>   // ETH_P_ARP = 0x0806
#include <linux/if_packet.h>  // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <openssl/dh.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <openssl/cryptoerr.h>

#pragma once

#define __CLIENT_MAIN 0
#define __CLIENT_SEND 1
#define __CLIENT_RECV 2
#define __CLIENT_CHAT 3

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
    char ip[16];
    int port;
    int fd;
    struct sockaddr_in s_addr;
    char secret[32];
} connection;

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

/**
*	Gets the MAC Address 
*	It does the thing, creates 12B of space on the heap
**/
unsigned char * get_mac(char * iface);

char * get_ip(char * iface);

/**
*	Formats the mac address
*	Creates 18B of space on the heap
**/
char * format_mac(unsigned char * mac);

/**
* Instant newline function. Why i made this i dont know.
*
**/
void newline();

/**
*	Prints the mac address for the given interface
*	Frees all memory at the end.
**/
void print_mac(char * iface);

void printConnection(connection * conn);

void handleErrors();