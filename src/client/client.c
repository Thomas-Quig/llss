#include "client.h"
#define nl newline
#define __MAX_BUFFER_SIZE 16384
#define __FRAG_SIZE 4

static pthread_t chat_threads[2];
static char _ohost_ip[16];
static char _orig_mac[6];
void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\nReceived user interrupt via ^C, safely exiting...\n");
        //set_mac(_global_conf._IFACE,_orig_mac);
        char cmd[128];
        memset(cmd,0,128);
        
        sprintf(cmd,"arp -d %s",_ohost_ip);
        system(cmd);
        printf("Recover original mac address with ping -I %s %s\n",_global_conf._IFACE,_ohost_ip);

        exit(0);
    }
}

int client_main(int argc, char ** argv, int mode)
{	
    memset(_ohost_ip,0,16);
    if(argc > 2)
        strncpy(_ohost_ip,argv[2],min(strlen(argv[2]),15));

	//printf("Original Mac: ");
	
    memcpy(_orig_mac,get_mac(_global_conf._IFACE),12);
	//print_mac(_global_conf._IFACE);nl();

    signal(SIGINT,sig_handler);

	switch(mode){
        case __CLIENT_MAIN:
            custom_test_code(argc,argv);
            break;
        case __CLIENT_RECV:
            //pong(establish_connection(argv[2],atoi(argv[3]),__CLIENT_RECV));
            recv_content(argv[2],atoi(argv[3]));
            break;
        case __CLIENT_SEND:
            if(access(argv[4],F_OK) != -1)
            {
                //ping(establish_connection(argv[2],atoi(argv[3]),__CLIENT_SEND));
                send_content(argv[2],atoi(argv[3]),argv[4],__SEND_FILE);
            }
            else
            {
                //ping(establish_connection(argv[2],atoi(argv[3]),__CLIENT_SEND));
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
    if(strcmp(argv[argc - 1],"-c"))
	    cleanup(_orig_mac,argv[2]);
    
	return 0;
}

void wizard()
{
    enum {GENERAL, CONFIGURE, INFO, EXECUTE} state = GENERAL;
    int exec_option = 0;
    while(state != EXECUTE)
    {
        
        if(state == GENERAL)
        {

        }
        else if(state == CONFIGURE)
        {
            if(access("./llss.conf",F_OK) != -1)
                configure("./llss.conf");
            else
                configure(NULL);
        }
    }

}

void print_wizard_options()
{
	printf("1. Get Current Mac Address\n");
	printf("2. Send message\n");
	printf("3. Send file\n");
    printf("4. Receive message or file\n");
    printf("5. Chat\n");
    printf("6. Configure settings\n");

    //If you happen to be compiling/editing this yourself, hello :)
    printf("7. Custom Test Code"); 
}

void configure(char * conf_path)
{
    if(conf_path != NULL && access(conf_path, F_OK) != -1)
    {
        FILE * f = fopen(conf_path,"r");
        fscanf(f,"%i\n%i\n%i\n%i\n%i\n%i",(_global_conf._DEBUG),(_global_conf._FUNCLIST),(_global_conf._SHUFFLE),(_global_conf._ENCRYPT),(_global_conf._CLEANUP),(_global_conf._LOG_SYS));
        printf("Params: %i,%i,%i,%i,%i,%i",(_global_conf._DEBUG),(_global_conf._FUNCLIST),(_global_conf._SHUFFLE),(_global_conf._ENCRYPT),(_global_conf._CLEANUP),(_global_conf._LOG_SYS));
    }
    else
    {
        char * param_qs[6] = {"Print debug statements","Print function calls","Shuffle MAC addresses","Encrypt messages","Cleanup arp at end of session", "Log system messages to file"};
        int * params[6] = {&(_global_conf._DEBUG),&(_global_conf._FUNCLIST),&(_global_conf._SHUFFLE),&(_global_conf._ENCRYPT),&(_global_conf._CLEANUP),&(_global_conf._DEBUG)};
        char _arg[8];
        int _bfr = -1;
        for(int i = 0; i < 6; i++)
        {
            _bfr = -1;
            printf("%s?:",param_qs[i]);
            fgets(_arg,8,stdin);
            _bfr = boolify(_arg);
            if(_bfr != -1)
                *(params[i]) = _bfr;
        }
        if(_global_conf._LOG_SYS)
        {
            char log_path[64];
            printf("File path for sys logs?:");
            fgets(log_path, 64,stdin);
            //_global_conf._DB_OUTPUT_FD = open(log_path, O_CREAT | O_TRUNC);
        }
    }
    
}

int custom_test_code(int argc, char ** argv)
{
	mac_change_loop();
	return 0;
}

void send_content(char * ip, int port, char * arg, int mode)
{
    connection * conn = establish_connection(ip,port,__CLIENT_SEND);
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
        char * next_macs = get_next_macs(__CLIENT_SEND);

        ssize_t tmp_sent = 0;
        size_t to_send = min(content_size - tot_sent,__FRAG_SIZE);

        tmp_sent = s_send(conn,content + tot_sent, to_send);
        tot_sent += tmp_sent;
        _sys_log("[SND]%d Bytes sent to %s\n\n",tmp_sent,conn -> ip);
        advance_mac(conn,next_macs,__ADV_SELF);

        int acked = 0;
        while(!(acked))
        {
            char response[65];
            ssize_t rf_resp = s_recv(conn,response,64);
            response[rf_resp] = '\0';
            _sys_log("[RSP] Response receieved...\n\"%s\"\n[End Response]\n\n",response);
            if(!strncmp(response,"ACK",3))
                acked = 1;
        }
        advance_mac(conn,next_macs,__ADV_OTHR);
    }
    s_send(conn,"ENDMSG",6);
    char endbuf[12];
    printf("Received final packet %d:\"%.12s\", process complete, cleaning up.\n",s_recv(conn,endbuf,12),endbuf);
}

void recv_content(char * ip, int port)
{
    //The IP here is technically unneccesary, but valuable for debugging.
    connection * conn = establish_connection(ip,port,__CLIENT_RECV);
	recv_loop(conn);
	close(conn -> fd); 
	free(conn);
}

int recv_loop(connection * conn)
{
    
    char rcv_buf[__FRAG_SIZE];
    memset(rcv_buf,0,__FRAG_SIZE);
    ssize_t bytes_rcvd;
    ssize_t bytes_rspd;
    do
    {
        char * next_macs = get_next_macs(__CLIENT_RECV);

        _sys_log("Waiting on data...\n");
        bytes_rcvd = s_recv(conn,rcv_buf,__FRAG_SIZE);
        _sys_log("[RCVD] Received %d bytes\n",bytes_rcvd);
        advance_mac(conn,next_macs,__ADV_OTHR);

        write(STDOUT_FILENO,rcv_buf,bytes_rcvd);
        
        char resp_buf[12];
        memset(resp_buf,0,12);
        sprintf(resp_buf,"ACK:%.4x",*((int *)rcv_buf));
        bytes_rspd = s_send(conn, resp_buf,strlen(resp_buf));
        //sendto(conn -> fd, sendbuf,strlen(sendbuf), 
        //    MSG_CONFIRM, (const struct sockaddr *)&cli_addr, len);
        
        advance_mac(conn,next_macs,__ADV_SELF);
    } while (strncmp(rcv_buf,"ENDMSG",6));
    
}

void cleanup(char * orig_mac, char * ip)
{
	set_mac(_global_conf._IFACE,orig_mac);
	char cmd[64];
	sprintf(cmd,"arp -d %s",ip);
	system(cmd);
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
	
	char _currmac[6] = {0x00,0x00,0x00,0x00,0x00,0x11};
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
			set_mac(_global_conf._IFACE,_currmac);
			printf("New Mac: ");
			print_mac(_global_conf._IFACE);nl();
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


void mac_change_loop()
{
	while(1){
		char _newmac[6];
        printf("Enter Mac: ");
		scanf("%x:%x:%x:%x:%x:%x",_newmac,_newmac + 1,_newmac + 2,_newmac + 3,_newmac + 4,_newmac + 5);
		printf("New Mac:%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",_newmac[0],_newmac[1],_newmac[2],_newmac[3],_newmac[4],_newmac[5]);
		//If you were on ssh, ssh gets hella bonked
		set_mac(_global_conf._IFACE,_newmac);
        printf("Confirm New: ");
		print_mac(_global_conf._IFACE); nl();
		//system("ifconfig | grep -A 5 'wlan0'");
		//printf("Exit? (y/n): ");
	}
}