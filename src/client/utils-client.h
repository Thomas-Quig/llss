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
#define __ADV_SELF 1
#define __ADV_OTHR 2
#define __ADVANCE_MACS 1

ssize_t s_send(connection * conn, char * data, size_t size);
ssize_t s_recv(connection * conn, char * data, size_t size);
int ping(connection * conn);
int pong(connection * conn);
/**
*   Changes your current mac address
*   Returns 0 on success, -1 otherwise
**/
int set_mac(char * iface, char * newMac);

void set_arp_cache(char * ip, char * _new_mac);

char * get_next_macs(int mode);
int advance_mac(connection * conn, char * macs, int who);