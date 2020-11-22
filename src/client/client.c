#include "client.h"
#define nl newline
#define __MAX_BUFFER_SIZE 16384

static pthread_t chat_threads[2];
static char _ohost_ip[16];

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\nReceived user interrupt via ^C, safely exiting...\n");
        char cmd[32];
        memset(cmd,0,32);
        sprintf(cmd,"arp -d %s",_ohost_ip);
        system(cmd);
        sprintf(cmd,"ping -c 1 %s",_ohost_ip);
        printf("Ensure arp has recovered with 'arp %s'\n",_ohost_ip);
        exit(0);
    }
}

void print_wizard_options()
{
	printf("1. Get Current Mac Address");
	printf("2. Change Mac Address (User Input)");
	printf("3. Change Mac Address (Conf File, Random)");
	printf("4. Change Mac Address (Conf File, Pregen)");
	printf("T. Run Custom Code");
}

int custom_test_code(int argc, char ** argv)
{
	send_content(argv[2],atoi(argv[3]),"./testfiles/test.txt",__SEND_FILE);
	return 0;
}

void mac_change_loop()
{
	do
	{
		uint8_t _newmac[6];
		char _newmac_pretty[13];
		scanf("%"SCNd8":%"SCNd8":%"SCNd8":%"SCNd8":%"SCNd8":%"SCNd8,_newmac,_newmac + 1,_newmac + 2,_newmac + 3,_newmac + 4,_newmac + 5);
		sprintf(_newmac_pretty,"%x:%x:%x:%x:%x:%x",_newmac[0],_newmac[1],_newmac[2],_newmac[3],_newmac[4],_newmac[5]);
		printf("New Mac: %s\n",_newmac_pretty);

		//If you were on ssh, ssh gets hella bonked
		set_mac(__IFACE,_newmac);
		print_mac(__IFACE); nl();
		system("ifconfig | grep -A 5 'wlan0'");
		printf("Exit? (y/n): ");
	}
	while(getc(stdin) != 'y');
}

void send_content(char * ip, int port, char * arg, int mode)
{
    connection * conn = establish_connection(ip,port,__CLIENT_SEND);
    printConnection(conn);    
    if(conn == NULL)
    {
        printf("Error: Connection could not be established\n");
        return;
    }

    if(mode == __SEND_FILE)
    {
        FILE * infile = fopen(arg,"r");
        if(infile == NULL)
        {
            printf("Coult not open file");
            return;
        }
        else
        {
            printf("Sending file \"%s\" to %s\n",arg,conn -> ip);
            //http://www.fundza.com/c4serious/fileIO_reading_all/index.html
            fseek(infile, 0L, SEEK_END);
            long file_size = ftell(infile);
            fseek(infile, 0L, SEEK_SET);
            long tot_bytes_sent = 0;
            while(tot_bytes_sent < file_size)
            {
                size_t content_size = min(__MAX_BUFFER_SIZE,file_size - tot_bytes_sent);
                char content[content_size];
                fread(content,content_size,1,infile);
                send_loop(conn,content,content_size);
                tot_bytes_sent += content_size;
            }
        }
    }
    else if(mode == __SEND_MESSAGE)
    {
        send_loop(conn,arg,strlen(arg));
    }
    else
    {
        printf("How\n");
        return;
    }
    
}

size_t send_loop(connection * conn, char * content, size_t content_size){
    size_t tot_sent = 0;
    while(tot_sent < content_size)
    {
        ssize_t tmp_sent = 0;
        size_t to_send = min(content_size - tot_sent,1024);
        printf("\n---------Packet--------\n");
        write(STDOUT_FILENO,(content + tot_sent),min(content_size - tot_sent,1024));
        fflush(stdout);
        printf("\n-------End Packet------\n\n");
        tmp_sent = sendto(conn -> fd, (const char *)(content + tot_sent), to_send, 
			MSG_CONFIRM, (const struct sockaddr *) &(conn -> s_addr),  
				sizeof(conn -> s_addr));
        printf("Packet sent, code %d\n",tmp_sent);
        int acked = 0;
        while(!(acked))
        {
            int len;
            char response[65];
            ssize_t rf_resp = recvfrom(conn -> fd, response, 64,  
					MSG_WAITALL, (struct sockaddr *) &(conn -> s_addr), 
					(socklen_t * )&len);
            response[rf_resp] = '\0';
            printf("Response: \"%s\"\n",response);
            acked = 1;
        }

        advance_macs(conn -> ip,__CLIENT_SEND);
    }
}

void chat(char * ip,int port)
{
	connection * conn = establish_connection(ip,port,__CLIENT_CHAT);
	chat_loop(conn);
	close(conn -> fd); 
	free(conn);
}

int chat_loop(connection * conn)
{
	
	uint8_t _currmac[6] = {0x00,0x00,0x00,0x00,0x00,0x11};
	while(1)
	{
        break;
		/**int n, len; 
		ssize_t bytes_sent = sendto(conn -> fd, (const char *)buf, size, 
			MSG_CONFIRM, (const struct sockaddr *) &(conn -> s_addr),  
				sizeof(conn -> s_addr)); 
		if(bytes_sent == -1)
		{
			perror("What");
			exit(1);
		}
		printf("%zd bytes sent.\n",bytes_sent); 
			
		n = recvfrom(conn -> fd, (char *)buf, sizeof(buf),  
					MSG_WAITALL, (struct sockaddr *) &(conn -> s_addr), 
					(socklen_t * )&len); 
		buf[n] = '\0'; 
		printf("Server Response: %s\n", buf); 
		char cont[2];
		printf("Continue? (y/n):");
		fflush(stdin);
		fgets(cont,2,stdin);
		cont[0] = toupper(cont[0]);
		printf("Cont:[%x][%x]",cont[0],cont[1]);

		//TODO Change this you idiot lol Stop being stubborn
		if(0 == 1)
		{
			break;
		}
		else
		{
			_currmac[5] += 17;
			set_mac(__IFACE,_currmac);
			printf("New Mac: ");
			print_mac(__IFACE);nl();
			continue;
		}**/
		
	}
	return 0;
}

void * chat_send(void * arg){
    connection * conn = (connection *)arg;
    char buf[512];
    memset(buf,0,512);
    size_t size = read(STDIN_FILENO,buf,511);
    
}

void * chat_recv(void * arg){

}


void recv_content(char * ip, int port)
{
    //The IP here is technically unneccesary
    connection * conn = establish_connection(ip,port,__CLIENT_RECV);
	recv_loop(conn);
	close(conn -> fd); 
	free(conn);
}

int recv_loop(connection * conn)
{
    printConnection(conn);
    char buf[1024];
    ssize_t bytes_rcvd;
    ssize_t bytes_rspd;
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    int len = sizeof(cli_addr);
    do
    {
        printf("Waiting on data...\n");
        bytes_rcvd = recvfrom(conn -> fd,(char *)buf, 1024,
        MSG_WAITALL,(struct sockaddr *)&cli_addr,(socklen_t *)&len);
        printf("\n---RCVD---\n");
        write(STDOUT_FILENO,buf,bytes_rcvd);
        printf("\n---ERCV---\n\n");
        char sendbuf[32];
        sprintf(sendbuf,"ACK %i",*((int *)buf));
        bytes_rspd = sendto(conn -> fd, sendbuf,strlen(sendbuf) + 1, 
            MSG_CONFIRM, (const struct sockaddr *)&cli_addr, len);
        advance_macs(conn -> ip,__CLIENT_RECV);
    } while (strncmp(buf,"ENDMSG",6));
    
}

void cleanup(uint8_t * orig, char * ip)
{
	set_mac(__IFACE,orig);
	char cmd[64];
	sprintf(cmd,"arp -d %s",ip);
	system(cmd);
}

int client_main(int argc, char ** argv, int mode)
{	
    memset(_ohost_ip,0,16);
    strncpy(_ohost_ip,argv[2],min(strlen(argv[2]),15));

	printf("Original Mac: ");
	uint8_t _orig_mac[6];
    memcpy(_orig_mac,get_mac(__IFACE),12);
	print_mac(__IFACE);nl();

    signal(SIGINT,sig_handler);

	switch(mode){
        case __CLIENT_MAIN:
            custom_test_code(argc,argv);
            break;
        case __CLIENT_RECV:
            recv_content(argv[2],atoi(argv[3]));
            break;
        case __CLIENT_SEND:
            if(access(argv[4],F_OK) -1)
            {
                send_content(argv[2],atoi(argv[3]),argv[4],__SEND_FILE);
            }
            else
            {
                send_content(argv[2],atoi(argv[3]),argv[4],__SEND_MESSAGE);
            }
            break;
        case __CLIENT_CHAT:
            chat(argv[2],atoi(argv[3]));
            break;
        default:
            printf("How\n");
            break;
    };
	cleanup(_orig_mac,argv[2]);
	return 0;
}