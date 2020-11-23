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

ssize_t ssend(connection * conn, char * data, size_t size);

/**
*   Changes your current mac address
*   Returns 0 on success, -1 otherwise
**/
int set_mac(char * iface, char * newMac);

void set_arp_cache(char * ip, char * _new_mac);

int advance_macs(connection * conn, int mode);