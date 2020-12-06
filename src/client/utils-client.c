#include "utils-client.h"
// Found on https://www.linuxquestions.org/questions/programming-9/how-to-change-mac-addres-via-c-code-801613/
ssize_t s_send(connection * conn, char * data, size_t size)
{
    _sys_log("s_send(%p,\"%.8s...%.8s\",%d)\n",conn,data,data + size - 8,size);
    close(conn -> fd);
    conn -> fd = socket(AF_INET,SOCK_DGRAM,0);
    (conn -> s_addr).sin_family = AF_INET; 
    (conn -> s_addr).sin_port = htons(conn -> port);
    //(conn -> s_addr).sin_addr.s_addr = inet_addr(conn -> ip);
    usleep(10000);
    return sendto(conn -> fd, (const char *)(data), size, MSG_CONFIRM,
            (const struct sockaddr *) &(conn -> s_addr),conn -> s_len);
}

ssize_t s_recv(connection * conn, char * data, size_t size)
{
    _sys_log("s_recv(%p,%p,%d)\n",conn,data,size);
    //return recv(conn -> fd, data, size - 1, MSG_WAITALL);
    close(conn -> fd);
    conn -> fd = socket(AF_INET,SOCK_DGRAM,0);
    (conn -> s_addr).sin_addr.s_addr = INADDR_ANY;
    if(bind(conn -> fd,(const struct sockaddr *)&(conn -> s_addr),conn -> s_len) == -1)
    {
        perror("rcv-bind");
        return -1;
    }
    ssize_t retval = recvfrom(conn -> fd, data, size, 
		MSG_WAITALL, (struct sockaddr *) &(conn -> s_addr),&(conn ->s_len));
    close(conn -> fd);
    conn -> fd = socket(AF_INET,SOCK_DGRAM,0);
    return retval;
}

ssize_t ds_exchange(connection * conn,int ds)
{
    ssize_t data_size;
    switch(conn -> _mode)
    {
        char buf[8];memset(buf,0,8);
        case __CLIENT_RECV:
            if(s_recv(conn,buf,sizeof(int)) == -1){
                perror("r-dsexch-recv:");
                return -1;
            }
            data_size = atoi(buf);
            if(s_send(conn,buf,strlen(buf)) == -1){
                perror("r-dsexch-send:");
                return -1;
            }
        case __CLIENT_SEND:
            sprintf(buf,"%i",ds);
            if(s_send(conn,buf,strlen(buf)) == -1){
                perror("r-dsexch-send:");
                return -1;
            }
            if(s_recv(conn,buf,sizeof(int)) == -1){
                perror("r-dsexch-recv:");
                return -1;
            }
            data_size = atoi(buf);
        default:
            return -1;
    } 
    return data_size;
}

int set_mac(char * iface, char * newMac)
{
    //char cmd[64];
    //memset(cmd,0,64);
    //sprintf(cmd,"ifconfig %s hw ether %.2x:%.2x:%.2x:%.2x:%.2x:%.2x",iface,newMac[0],newMac[1],newMac[2],newMac[3],newMac[4],newMac[5]);
    //system(cmd);
    _sys_log("set_mac(%s,%.2x:%.2x:%.2x:%.2x:%.2x:%x)\n",iface,newMac[0],newMac[1],newMac[2],newMac[3],newMac[4],newMac[5]);
    //return EXIT_SUCCESS;
    //printf("%d:%d:%d:%d:%d:%d\n",newMac[0],newMac[1],newMac[2],newMac[3],newMac[4],newMac[5]);
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
        //printf("explain-ioctl: %s\n",explain_errno_ioctl(errno,s,SIOCSIFHWADDR, &ifr));
        return EXIT_FAILURE;
	  }
    
}

void set_arp_cache(char * ip, char * _new_mac)
{
    _sys_log("set_arp_cache(%s,%.2x:%.2x:%.2x:%.2x:%.2x:%.2x)\n",ip,_new_mac[0],_new_mac[1],_new_mac[2],_new_mac[3],_new_mac[4],_new_mac[5]);
	char cmd[64];
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

char * get_next_macs(int mode)
{
    _sys_log("get_next_macs(%i)\n",mode);
    char * new_macs = malloc(12);
    char * my_new_mac = new_macs;
    char * ot_new_mac = new_macs + 6;
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
    return new_macs;
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