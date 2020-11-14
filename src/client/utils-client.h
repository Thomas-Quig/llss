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
#include "../utils/utils.h"

/**
*   Changes your current mac address
*   Returns 0 on success, -1 otherwise
**/
int set_mac(char * iface, uint8_t * newMac);


/**
*   Given a new mac address, sends a gratuitious arp request to tell all services the new mac address.
*   This is an INSECURE option, and can be tracked easily with proper wireshark filtering.
*   This is also a costly option, as sending a gratuitious arp every packet is super costly.
**/
int send_garp(uint8_t * newMac);

void set_arp_cache(char * ip, uint8_t _new_mac);

/**
 *  Generates next mac address for given ip address and the shared secret.
 *  
 **/
uint8_t * get_next_mac(char * ip, char shared_secret[32]);