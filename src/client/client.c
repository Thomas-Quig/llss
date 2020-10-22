#include "client.h"
#define nl newline

void print_wizard_options()
{
	printf("1. Get Current Mac Address");
	printf("2. Change Mac Address (User Input)");
	printf("3. Change Mac Address (Conf File, Random)")
	printf("4. Change Mac Address (Conf File, Pregen)")
}

int client_main(int argc, char ** argv)
{	
	printf("Original Mac: ");
	print_mac(wlan0); nl();
	while(getc(stdin) == 'y')
	{
		uint8_t _newmac[6];
		char _newmac_pretty[13];
		scanf("%c:%c:%c:%c:%c:%c",_newmac,_newmac + 1,_newmac + 2,_newmac + 3,_newmac + 4,_newmac + 5);
		sprintf(_newmac_pretty,"%x:%x:%x:%x:%x:%x",_newmac[0],_newmac[1],_newmac[2],_newmac[3],_newmac[4],_newmac[5]);
		printf("New Mac: %s\n",_newmac_pretty);

		//If you were on ssh, ssh gets hella bonked
		change_mac(_newmac); 
		print_mac("wlan0"); nl();
		system("ifconfig | grep -B 5 'wlan0'")
	}
	

	return 0;
}