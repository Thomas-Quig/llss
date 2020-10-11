#include "utils-client.h"

// Found on https://www.linuxquestions.org/questions/programming-9/how-to-change-mac-addres-via-c-code-801613/

int change_mac(uint8_t * newMac)
{
	struct ifreq ifr;
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	assert(s != -1);

	strcpy(ifr.ifr_name, "wlan0");
	ifr.ifr_hwaddr.sa_data[0] = newMac[0];
	ifr.ifr_hwaddr.sa_data[1] = newMac[1];
	ifr.ifr_hwaddr.sa_data[2] = newMac[2];
	ifr.ifr_hwaddr.sa_data[3] = newMac[3];
	ifr.ifr_hwaddr.sa_data[4] = newMac[4];
	ifr.ifr_hwaddr.sa_data[5] = newMac[5];
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	if(ioctl(s, SIOCSIFHWADDR, &ifr) == -1){
		perror("IOCTL");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}