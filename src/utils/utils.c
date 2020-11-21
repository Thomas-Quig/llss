#include "utils.h"
//Modified from https://www.binarytides.com/c-program-to-get-mac-address-from-interface-name-on-linux/

connection * establish_connection(char * addr, int port, int mode)
{
	connection * ret = malloc(sizeof(connection));
	strncpy(ret -> ip,addr,strlen(addr));
	(ret -> ip)[strlen(addr)] = '\0';
	ret -> port = port;
    memset(&(ret -> s_addr),0,sizeof(ret -> s_addr));
  
    // Creating socket file descriptor 
    if ((ret -> fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
		free(ret);
        return NULL;
    }
    memset(&(ret ->s_addr), 0, sizeof(ret -> s_addr)); 
      
    // Filling server information 
    (ret -> s_addr).sin_family = AF_INET; 
    (ret -> s_addr).sin_port = htons(port);
    if(mode == __CLIENT_RECV){
        (ret -> s_addr).sin_addr.s_addr = INADDR_ANY;
        if(bind(ret -> fd, (const struct sockaddr *)&(ret -> s_addr),
            sizeof(ret -> s_addr)) == -1)
        {
            perror("rcv-bind failed");
            printf("errno: %i",errno);
            exit(EXIT_FAILURE);
        }
    }
    else{
        (ret -> s_addr).sin_addr.s_addr = inet_addr(ret -> ip);
    }
    
    strncpy(ret -> secret,estab_shared_secret(ret,mode),32);
	return ret;
}

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

char * get_ip(char * iface)
{
    int fd;
    struct ifreq ifr;
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);
    ioctl(fd, SIOCGIFADDR, &ifr);
    close(fd);
    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
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


// Based on https://wiki.openssl.org/index.php/Diffie_Hellman
char * estab_shared_secret(connection * conn, int mode)
{
    char * ret = malloc(32);
    strncpy(ret,"0123456789ABCDEF0123456789ABCDEF",32);
    printf("Shared Secret: %.*s\n",32,ret);
    return ret;
    DH *mykey;
    int codes;
    int secret_size;

    /* Generate the parameters to be used */
    if(NULL == (mykey = DH_new())) handleErrors();
    if(1 != DH_generate_parameters_ex(mykey, 1024, DH_GENERATOR_2, NULL)) handleErrors();

    if(1 != DH_check(mykey, &codes)) handleErrors();
    if(codes != 0)
    {
        /* Problems have been found with the generated parameters */
        /* Handle these here - we'll just abort for this example */
        printf("DH_check failed\n");
        abort();
    }

    /* Generate the public and private key pair */
    if(1 != DH_generate_key(mykey)) handleErrors();

    /* Send the public key to the peer.
    * How this occurs will be specific to your situation (see main text below) */
    int keysize = BN_num_bytes(DH_get0_pub_key(mykey));
    char * ohost = malloc(keysize);  
    int len;

    //Yes this is repetitive code I dont give a shit how else do i do it.
    if(mode == __CLIENT_SEND)
    {
        char pubbuf[keysize];
        if(-1 == BN_bn2bin(DH_get0_pub_key(mykey),pubbuf)) handleErrors();

        ssize_t key_sent = sendto(conn -> fd,pubbuf, keysize, 
			MSG_CONFIRM, (const struct sockaddr *) &(conn -> s_addr),  
			sizeof(conn -> s_addr));
        
        ssize_t key_recv = recvfrom(conn -> fd, ohost, keysize,  
					MSG_WAITALL, (struct sockaddr *) &(conn -> s_addr), 
					(socklen_t * )&len); 
    }
    else if(mode == __CLIENT_RECV)
    {
        ssize_t key_recv = recvfrom(conn -> fd, ohost, keysize,  
					MSG_WAITALL, (struct sockaddr *) &(conn -> s_addr), 
					(socklen_t * )&len);

        char pubbuf[keysize];
        if(-1 == BN_bn2bin(DH_get0_pub_key(mykey),pubbuf)) handleErrors();

        ssize_t key_sent = sendto(conn -> fd,pubbuf, keysize, 
			MSG_CONFIRM, (const struct sockaddr *) &(conn -> s_addr),  
			sizeof(conn -> s_addr)); 
    }
    
    /* Receive the public key from the peer. In this example we're just hard coding a value */
    
    BIGNUM *ohostkey = NULL;
    if(0 == (BN_dec2bn(&ohostkey, ohost))) handleErrors();

    /* Compute the shared secret */
    unsigned char *secret;
    if(NULL == (secret = OPENSSL_malloc(sizeof(unsigned char) * (DH_size(mykey))))) handleErrors();

    if(0 > (secret_size = DH_compute_key(secret, ohostkey, mykey))) handleErrors();

    /* Do something with the shared secret */
    /* Note secret_size may be less than DH_size(mykey) */
    printf("The shared secret is:\n");
    BIO_dump_fp(stdout, secret, secret_size);

    return secret;
}

void printConnection(connection * conn)
{
    printf("Connection %p\n",conn);
    printf("Addr: %s\n",conn -> ip);
    printf("Port: %i\n",conn ->port);
    printf("FD: %i\n",conn -> fd);
    printf("Secret: %.*s\n",32,conn ->secret);
    printf("Saddr: %p\n",conn -> s_addr);
}


void handleErrors()
{
    printf("OPENSSL ERROR, EXITING\n");
    perror("OSSL-ERRNO");
}