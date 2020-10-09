#include "client.h"

int main(int argc, char ** argv)
{
	print_mac();
	uint8_t _newmac[6];
	char _newmac_pretty[13];
	scanf("%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",_newmac,_newmac + 1,_newmac + 2,_newmac + 3,_newmac + 4,_newmac + 5);
	sprintf(_newmac_pretty,"%x:%x:%x:%x:%x:%x",_newmac[0],_newmac[1],_newmac[2],_newmac[3],_newmac[4],_newmac[5]);
	printf("New Mac: %s\n",_newmac_pretty);
	change_mac(_newmac);
	print_mac();
	return 0;
}
