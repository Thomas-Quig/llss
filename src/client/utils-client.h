
#include "../utils/utils.h"


#define __ADV_SELF 1
#define __ADV_OTHR 2
#define __ADVANCE_MACS 1

/**
*   Changes your current mac address
*   Returns 0 on success, -1 otherwise
**/
int set_mac(char * iface, char * newMac);
ssize_t ds_exchange(connection * conn,int ds);
void set_arp_cache(char * ip, char * _new_mac);

char * get_next_macs(int mode,char * mac_buf);
int advance_mac(connection * conn, char * macs, int who);