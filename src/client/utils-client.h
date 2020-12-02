
#include "../utils/utils.h"

#define __IFACE "wlan0"
#define __ADV_SELF 1
#define __ADV_OTHR 2
#define __ADVANCE_MACS 1

ssize_t s_send(connection * conn, char * data, size_t size);
ssize_t s_recv(connection * conn, char * data, size_t size);

/**
*   Changes your current mac address
*   Returns 0 on success, -1 otherwise
**/
int set_mac(char * iface, char * newMac);

void set_arp_cache(char * ip, char * _new_mac);

char * get_next_macs(int mode);
int advance_mac(connection * conn, char * macs, int who);