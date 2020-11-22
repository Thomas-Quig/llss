#include "utils-client.h"

// Found on https://www.linuxquestions.org/questions/programming-9/how-to-change-mac-addres-via-c-code-801613/

int set_mac(char * iface, uint8_t * newMac)
{
    printf("Setting own MAC to %hhx:%hhx:%hhx:%hhx:%hhx:%hhx",newMac[0],newMac[1],newMac[2],newMac[3],newMac[4],newMac[5]);
    struct ifreq ifr;
    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    assert(s != -1);

    strcpy(ifr.ifr_name, iface);
    ifr.ifr_hwaddr.sa_data[0] = newMac[0];
    ifr.ifr_hwaddr.sa_data[1] = newMac[1];
    ifr.ifr_hwaddr.sa_data[2] = newMac[2];
    ifr.ifr_hwaddr.sa_data[3] = newMac[3];
    ifr.ifr_hwaddr.sa_data[4] = newMac[4];
    ifr.ifr_hwaddr.sa_data[5] = newMac[5];
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    if(ioctl(s, SIOCSIFHWADDR, &ifr) == -1){
        perror("IOCTL");
        printf("ioctl-errno: %i\n",ioctl);
        return EXIT_FAILURE;
	  }

	  
}

void set_arp_cache(char * ip, uint8_t * _new_mac)
{
	char cmd[64];
	sprintf(cmd,"arp -s %s %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",ip,_new_mac[0],_new_mac[1],_new_mac[2],_new_mac[3],_new_mac[4],_new_mac[5]);
	printf("%s",cmd);
	system(cmd);
  
}

uint8_t * get_next_mac(char * ip, char shared_secret[32])
{

}

void seed_mac_adv(char shared_secret[32])
{
    unsigned int * sec_int = (unsigned int *)shared_secret;

    //DEBUG, REMOVE LATER
    for(int i = 0; i < 8; i++){printf("%i -> %i\n",i,sec_int[i]);}
    srand(sec_int[0]);
    printf("Pairing Code: %i\n",rand() % 1000000);
}

int advance_macs(char * ip, int mode)
{
    uint8_t my_new_mac[6];
    uint8_t ot_new_mac[6];
    for(int i = 0; i < 6; i++)
    {
        if(mode == __CLIENT_SEND)
        {
            my_new_mac[i] = (uint8_t)(rand() % 255);
            ot_new_mac[i] = (uint8_t)(rand() % 255);
        }
        else if(mode == __CLIENT_RECV)
        {
            ot_new_mac[i] = (uint8_t)(rand() % 255);
            my_new_mac[i] = (uint8_t)(rand() % 255);
        }
        else
        {
            return -1; //This should be impossible but whatever.
        }
    }
    printf("LOC|%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\nOTH|%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",my_new_mac[0],my_new_mac[1],my_new_mac[2],my_new_mac[3],my_new_mac[4],my_new_mac[5],
                                                            ot_new_mac[0],ot_new_mac[1],ot_new_mac[2],ot_new_mac[3],ot_new_mac[4],ot_new_mac[5]);
    set_mac(__IFACE,my_new_mac);
    set_arp_cache(ip,ot_new_mac);
}