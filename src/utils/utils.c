#include "utils.h"
//Modified from https://www.binarytides.com/c-program-to-get-mac-address-from-interface-name-on-linux/

config _global_conf = 
{/*_VERBOSE*/ 0 , /*_FUNCLIST*/ 0 , /*_SHUFFLE*/ 1 , /*_ENCRYPT*/ 1 ,
/*_CLEANUP*/ 0 , /*_LOG_SYS*/ 0 , /*_CHECK_FILE*/ 0, /*_FRAG_SIZE*/ 1024, 
/*_CSTMSEED*/ -1 , /*_OUTPUT_FD*/ 1 , /*_DB_OUTPUT_FD*/ 1, /*_IFACE*/ "wlan0"};

static const char g_dh2048_pm[] = 
    "-----BEGIN DH PARAMETERS-----\n"
    "MIIBCAKCAQEA+j881N4TQijS87QR0pOO0f1wkJ7X6myrOU06n4hRr1pR30epHzjB\n"
    "Ytku4sW6i3SVjvNsFBcHdo42/zTH6BzkwxoRjogrDGyQ5zX0bUep5cAPC2Ktx5I3\n"
    "XFptTUctY4iLB/AfzcwzXXbfroLvzIB0tMS06psMgtbcHgPdbS0SyoSIK0Khh1rk\n"
    "W/bpLi65lStCNS2xNaljcuH6W5VHHHcjIBrdCjZtnjf9woAo5KzcVK5c9b+pMqQn\n"
    "FJUh3/PT4A+m6Q2z8HvxpirmovYl7wZEbxR7OF9ZHi3X3YeglOh3foQDIRjKAG2W\n"
    "fKPo99jRhG4Tao/XJldhoFai9ACCQIUhiwIBAg==\n"
    "-----END DH PARAMETERS-----";

ssize_t s_send(connection * conn, char * data, size_t size)
{
    _sys_log("s_send(%p,\"%.8s...\",%du)\n",conn,data,size);
    close(conn -> fd);
    conn -> fd = socket(AF_INET,SOCK_DGRAM,0);
    (conn -> s_addr).sin_family = AF_INET; 
    (conn -> s_addr).sin_port = htons(conn -> port);
    (conn -> s_addr).sin_addr.s_addr = inet_addr(conn -> ip);
    //(conn -> s_addr).sin_addr.s_addr = inet_addr(conn -> ip);
    usleep(10000);
    ssize_t retval = sendto(conn -> fd, (const char *)(data), size, MSG_CONFIRM,
            (const struct sockaddr *) &(conn -> s_addr),conn -> s_len);
    return retval;
}

ssize_t s_recv(connection * conn, char * data, size_t size)
{
    _sys_log("s_recv(%p,%p,%du)\n",conn,data,size);
    //return recv(conn -> fd, data, size - 1, MSG_WAITALL);
    close(conn -> fd);
    conn -> fd = socket(AF_INET,SOCK_DGRAM,0);
    (conn -> s_addr).sin_addr.s_addr = INADDR_ANY;
    //(conn -> s_addr).sin_port = conn -> port;
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

connection * establish_connection(char * addr, int port, int mode)
{
	connection * ret = calloc(sizeof(connection),1);
    ret -> mode = mode;
	strncpy(ret -> ip,addr,strlen(addr));
	ret -> port = port;
    if(ret -> port == -1)
    {
        port = 7755;
    }
    // Creating socket file descriptor 
    if ((ret -> fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("fd fail"); 
		free(ret);
        return NULL;
    }
    
    // Filling server information 
    (ret -> s_addr).sin_family = AF_INET; 
    (ret -> s_addr).sin_port = htons(port);
    (ret -> s_addr).sin_addr.s_addr = inet_addr(ret -> ip);
    ret -> s_len = sizeof(ret -> s_addr);

    //If you arent encrypting, then you aren't being super duper secure, so just srand with the port.
    if(_global_conf._ENCRYPT)
    {
        strncpy(ret -> secret,estab_shared_secret(ret,mode),32);
        srand(atoi(ret -> secret));
    }
    else
    {
        srand(port);
    }
	return ret;
}

int _sys_log(const char * format,...)
{
    va_list args;
    va_start(args, format);
    //struct timespec spec;
    //clock_gettime(CLOCK_REALTIME, &spec);
    //long ms = spec.tv_nsec / 1000 + spec.tv_sec;
    //dprintf(_global_conf._DB_OUTPUT_FD,"[%u] ",ms);
    if(_global_conf._VERBOSE)
    {
        int result = vdprintf(_global_conf._DB_OUTPUT_FD, format,args);
        va_end(args);
        return result;
    }
    va_end(args);
    return 0;
}

char * format_mac(unsigned char * mac)
{
	char * ret = malloc(13);
	sprintf(ret,"%.2x:%.2x:%.2x:%.2x:%.2x:%.2x" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return ret;
}

int boolify(char * input)
{   
    char c1 = tolower(input[0]);
    if(c1 != 'n' && c1 != '0' && c1 != 'f' && c1 != 'y' && c1 != 't' && c1 != '1')
        return -1;
    else
        return (c1 == 'y') || (c1 == 't') || (c1 == '1');
}


// Based on https://wiki.openssl.org/index.php/Diffie_Hellman
char * estab_shared_secret(connection * conn, int mode)
{
    _sys_log("estab_shared_secret(%p,%i)\n",conn,mode);
    //char * ret = malloc(32);
    //strncpy(ret,"0123456789ABCDEF0123456789ABCDEF",32);
    //return ret;
    
    int codes;
    int secret_size;

    /* Generate the parameters to be used */
    //if(NULL == (mykey = DH_new())) handleErrors();
    FILE * fp = fopen("dhparams.pem","r");
    DH * mykey = PEM_read_DHparams(fp,NULL,NULL,NULL);
    if(mykey == NULL) handleErrors(__LINE__);

    if(1 != DH_check(mykey, &codes)) handleErrors(__LINE__);
    if(codes != 0)
    {
        /* Problems have been found with the generated parameters */
        /* Handle these here - we'll just abort for this example */
        printf("DH_check failed\n");
        handleErrors(__LINE__);
        return NULL;
    }

    /* Generate the public and private key pair */
    if(1 != DH_generate_key(mykey)) handleErrors(__LINE__);

    /* Send the public key to the peer.
    * How this occurs will be specific to your situation (see main text below) */
    int keysize = BN_num_bytes(DH_get0_pub_key(mykey));
    if(keysize == -1) handleErrors(__LINE__);
    char * ohost = malloc(keysize);  
    int len;

    //Yes this is repetitive code I dont give a shit how else do i do it.
    if(mode == __CLIENT_SEND)
    {
        char pubbuf[keysize];
        if(-1 == BN_bn2bin(DH_get0_pub_key(mykey),pubbuf)) handleErrors(__LINE__);

        ssize_t key_sent = s_send(conn,pubbuf,keysize);
        if(key_sent == -1) goto keyexch_error;
        ssize_t key_recv = s_recv(conn,ohost,keysize);
    }
    else if(mode == __CLIENT_RECV)
    {
        ssize_t key_recv = s_recv(conn,ohost,keysize);

        char pubbuf[keysize];
        if(-1 == BN_bn2bin(DH_get0_pub_key(mykey),pubbuf)) handleErrors(__LINE__);

        ssize_t key_sent = s_send(conn,pubbuf,keysize);
        if(key_sent == -1) goto keyexch_error;
    }
    
    /* Receive the public key from the peer. In this example we're just hard coding a value */
    
    BIGNUM *ohostkey = NULL;
    if(0 == (BN_bin2bn(ohost,keysize,ohostkey))) handleErrors(__LINE__);

    /* Compute the shared secret */
    unsigned char *secret;
    if(NULL == (secret = OPENSSL_malloc(sizeof(unsigned char) * (DH_size(mykey))))) handleErrors(__LINE__);

    if(0 > (secret_size = DH_compute_key(secret, ohostkey, mykey))) handleErrors(__LINE__);

    /* Do something with the shared secret */
    /* Note secret_size may be less than DH_size(mykey) */
    printf("The shared secret(%i) is:",secret_size);
    for(int i = 0; i < secret_size; i++)
    printf("%.2x",secret[i]);
    BIO_dump_fp(stdout, secret, secret_size);

    return secret;

    dh_error:
        printf("openssl-dh error, exiting...\n");                                             
        return NULL;
    keyexch_error:
        printf("keyexchange error, exiting...\n");
        return NULL;
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors(__LINE__);

    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors(__LINE__);

    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors(__LINE__);
    ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors(__LINE__);
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors(__LINE__);

    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors(__LINE__);

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors(__LINE__);
    plaintext_len = len;

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors(__LINE__);
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

char * decrypt_msg(char * key,char * msg){

}

void printConnection(connection * conn)
{
    printf("Connection %p\n",conn);
    printf("Addr: %s\n",conn -> ip);
    printf("Port: %i\n",conn ->port);
    printf("FD: %i\n",conn -> fd);
    printf("Secret: %.*s\n",32,conn ->secret);
    printf("Saddr: %p\n",&(conn -> s_addr));
}


void handleErrors(int line)
{
    printf("OPENSSL ERROR LINE %i, EXITING\n",line);
    ERR_print_errors_fp(stderr);
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

ssize_t save_config(char * fname)
{
    for(int i = 0; i < 10; i++)
        printf("TODO MOVE THIS CODE OVER FROM WIZARD\n");
}