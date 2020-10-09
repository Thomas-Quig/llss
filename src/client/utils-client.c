#include "utils-client.h"

// Found on https://www.linuxquestions.org/questions/programming-9/how-to-change-mac-addres-via-c-code-801613/

char * parse_mac


int change_mac(char * newMac)
{
	int main(int argc, char **argv) {
	struct ifreq ifr;
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	assert(s != -1);

	strcpy(ifr.ifr_name, "eth0");
	ifr.ifr_hwaddr.sa_data[0] = newMac[0];
	ifr.ifr_hwaddr.sa_data[1] = newMac[1];
	ifr.ifr_hwaddr.sa_data[2] = newMac[2];
	ifr.ifr_hwaddr.sa_data[3] = newMac[3];
	ifr.ifr_hwaddr.sa_data[4] = newMac[4];
	ifr.ifr_hwaddr.sa_data[5] = newMac[5];
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	assert(ioctl(s, SIOCSIFHWADDR, &ifr) != -1);

	return EXIT_SUCCESS;
}

//Modified from https://www.binarytides.com/c-program-to-get-mac-address-from-interface-name-on-linux/
unsigned char * get_mac(char * iface)
{
	int fd;
	struct ifreq ifr;
	unsigned char *mac;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

	ioctl(fd, SIOCGIFHWADDR, &ifr);

	close(fd);
	
	mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
	unsigned char * ret = malloc(6);
	memcpy((void *)ret,(void *)mac,6);
	return ret;	
}

char * format_mac(unsigned char * mac)
{
	char * ret = malloc(18);
	sprintf(ret,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return ret;
}

void print_mac()
{
	unsigned char * mac = get_mac("wlan0");
	char * formatted = format_mac(mac);
	printf("%s",formatted);
	free(mac);
	free(formatted);
}
