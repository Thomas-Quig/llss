#include "utils-client.h"
// Found on https://www.linuxquestions.org/questions/programming-9/how-to-change-mac-addres-via-c-code-801613/
int set_mac(char * iface, char * newMac)
{
    //char cmd[64];
    //memset(cmd,0,64);
    //sprintf(cmd,"ifconfig %s hw ether %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",iface,newMac[0],newMac[1],newMac[2],newMac[3],newMac[4],newMac[5]);
    //system(cmd);
    _sys_log("set_mac(%s,%.2x:%.2x:%.2x:%.2x:%.2x:%x)\n",iface,newMac[0],newMac[1],newMac[2],newMac[3],newMac[4],newMac[5]);
    struct ifreq ifr;
    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    assert(s != -1);
    memset(&(ifr.ifr_name),0,IF_NAMESIZE);
    strncpy(ifr.ifr_name, "wlan0",min(strlen(iface),5));
    for(int i = 0; i < 6; i++)
        ifr.ifr_hwaddr.sa_data[i] = newMac[i];

    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    if(ioctl(s, SIOCSIFHWADDR, &ifr) == -1){
        perror("sm-IOCTL");
        return EXIT_FAILURE;
	  }
    
}


void set_arp_cache(char * ip, char * _new_mac)
{
    _sys_log("set_arp_cache(%s,%.2x:%.2x:%.2x:%.2x:%.2x:%.2x)\n",ip,_new_mac[0],_new_mac[1],_new_mac[2],_new_mac[3],_new_mac[4],_new_mac[5]);
	char cmd[64];memset(cmd,0,64);
	sprintf(cmd,"arp -s %s %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",ip,_new_mac[0],_new_mac[1],_new_mac[2],_new_mac[3],_new_mac[4],_new_mac[5]);
	//printf("cmd \"%s\"\n",cmd);
	system(cmd);
}

char safe_rand(int i)
{
    char c = (char)(rand() % 255);
    if(i == 0){
        c &= 0xFE;
    }
    return c;
}

char * get_next_macs(int mode, char * mac_buf)
{
    _sys_log("get_next_macs(%i)\n",mode);
    char * my_new_mac = mac_buf;
    char * ot_new_mac = mac_buf + 6;
    for(int i = 0; i < 6; i++)
    {
        if(mode == __CLIENT_SEND)
        {
            my_new_mac[i] = safe_rand(i);
            ot_new_mac[i] = safe_rand(i);
        }
        else if(mode == __CLIENT_RECV)
        {
            ot_new_mac[i] = safe_rand(i);
            my_new_mac[i] = safe_rand(i);
        }
        else
        {
            return NULL; //This should be impossible but whatever.
        }
    }
    _sys_log("get_next_macs(Self: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x, Othr: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x)\n",
    my_new_mac[0],my_new_mac[1],my_new_mac[2],my_new_mac[3],my_new_mac[4],my_new_mac[5],
    ot_new_mac[0],ot_new_mac[1],ot_new_mac[2],ot_new_mac[3],ot_new_mac[5],ot_new_mac[5]);
    return mac_buf;
}

int advance_mac(connection * conn, char *macs, int who)
{
    if(!_global_conf._SHUFFLE)
        return 0;
    char *my_new_mac = macs;
    char *ot_new_mac = macs + 6;
    _sys_log("advance_mac(%p,%p,%i)\n",conn,macs,who);
    if(who == __ADV_SELF)
    {
        //close(conn -> fd);
        set_mac(_global_conf._IFACE,my_new_mac);
        //conn -> fd = socket(AF_INET, SOCK_DGRAM, 0);
        /**if(conn -> mode == __CLIENT_RECV)
        {
            (conn -> s_addr).sin_addr.s_addr = INADDR_ANY;
            if(bind(conn -> fd,(const struct sockaddr *)&(conn -> s_addr),conn -> s_len) == -1)
            {
                perror("advance-bind");
            }
        }**/
    }
    else if(who == __ADV_OTHR)
    {
        char * ip = conn -> ip;
        set_arp_cache(ip,ot_new_mac);
    }
    return 0;
}