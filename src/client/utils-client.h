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
#include <pthread.h>
#include "../utils/utils.h"

#define __IFACE "wlan0"
/**
*   Changes your current mac address
*   Returns 0 on success, -1 otherwise
**/
int set_mac(char * iface, uint8_t * newMac);

void set_arp_cache(char * ip, uint8_t _new_mac);


/**
 *  Generates next mac address for given ip address and the shared secret.
 *  
 **/
uint8_t * get_next_mac(char * ip, char shared_secret[32]);

void seed_mac_adv(char shared_secret[32]);
int advance_macs(char ** ips, int mode);