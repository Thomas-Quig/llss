#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if_arp.h>
#include <net/if.h>

int change_mac(char * newMac);

unsigned char * get_mac(char * iface);

char * format_mac(unsigned char * mac);

void print_mac();
