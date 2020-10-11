#include "client.h"
int client_main(int argc, char ** argv)
{
	print_mac();
	printf("\n");
	uint8_t _newmac[6];
	char _newmac_pretty[13];
	scanf("%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",_newmac,_newmac + 1,_newmac + 2,_newmac + 3,_newmac + 4,_newmac + 5);
	sprintf(_newmac_pretty,"%x:%x:%x:%x:%x:%x",_newmac[0],_newmac[1],_newmac[2],_newmac[3],_newmac[4],_newmac[5]);
	printf("New Mac: %s\n",_newmac_pretty);

	//If you were on ssh, ssh gets hella bonked
	change_mac(_newmac); 
	print_mac();
	printf("\n");
	return 0;
}
