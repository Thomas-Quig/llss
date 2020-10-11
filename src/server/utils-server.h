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

/**
* Handles the arp packet update, records the information for debugging purposes, then forwards the packet.
*
**/
int handle_arp_update(char * buf);

