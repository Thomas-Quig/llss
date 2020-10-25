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

#pragma once

/**
 *  A UDP "Connection" consists of the IP (for easy printing), the port (for easy printing), the socket fd (so it can be used), 
 * and the sockaddr_in for actually sending stuff. Its not actually a connection as this is UDP, but I am calling it a connection because it has all the
 * makings of one without it actually being connected.
 * @param ip the ip that you are connecting to, space for full "AAA.BBB.CCC.DDD\x00"
 * @param port the port you want to send UDP packets to.
 * @param fd the file descriptor connected to the socket
 * @param s_addr the socket address that you be sending packets to.
 **/
typedef struct _connection{
    char ip[16];
    int port;
    int fd;
    struct sockaddr_in s_addr;
} connection;

/**
 * Establishes a "connection" with the given address and port, passes ip and port into the connection if it is needed.
 * Returns on the heap, so it needs to be freed.
 * 
 **/
connection * establish_connection(char * addr, int port);

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
