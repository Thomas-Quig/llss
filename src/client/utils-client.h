#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <assert.h>
#include <libnet.h>

typedef struct _arp_packet{

} arp_packet;


/**
*   Changes your current mac address
*   Returns 0 on success, -1 otherwise
**/
int change_mac(uint8_t * newMac);


/**
*   Given a new mac address, sends 
*
**/
int update_arp(uint8_t * newMac);

/**
*   Generate a gratuitious ARP update packet. Returns the struct
*
**/
arp_packet generate_arp_update(uint8_t * newMac);

/**
* Takes the arp_packet, turns it into a char *, and sends it on the broadcast channel.
* Cleans up and handles errors
**/
int send_arp(arp_packet packet, size_t len);

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
*	Prints the mac address
*	Frees all memory at the end.
**/
void print_mac();
