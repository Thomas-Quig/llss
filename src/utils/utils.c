#include "utils.h"
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
	char * ret = malloc(13);
	sprintf(ret,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return ret;
}

void newline()
{
	puts("\n");
	fflush(stdout);
}

void print_mac(char * iface)
{
	unsigned char * mac = get_mac(iface);
	char * formatted = format_mac(mac);
	printf("%s",formatted);
	free(mac);
	free(formatted);
}
