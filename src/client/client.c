#include "client.h"

static pthread_t chat_threads[2];
static char _target_ip[16];
static char _orig_mac[6];
int _lvn = 1,_mvn = 1,_rvn = 2;
void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\nReceived user interrupt via ^C, safely exiting...\n");
        //set_mac(_global_conf._IFACE,_orig_mac);
        char cmd[128];
        memset(cmd,0,128);
        
        sprintf(cmd,"arp -d %s",_target_ip);
        system(cmd);
        printf("Recover original mac address with ping -I %s %s\n",_global_conf._IFACE,_target_ip);

        exit(0);
    }
}

int client_main(int argc, char ** argv)
{	
    //Signal Handler for SIGINT
    signal(SIGINT,sig_handler);

    //Initial configuration, is overwritten by parse-args and any future configs.
    configure("./llss.conf");

    args a;
    parse_args(&a,argc,argv);
    if(strlen(a._conf_path) > 0){
        configure(a._conf_path);
    }

    memset(_target_ip,0,16);
    strncpy(_target_ip,a._target_ip,min(strlen(a._target_ip),15));

    memcpy(_orig_mac,get_mac(_global_conf._IFACE),12);


	switch(a._mode){
        case __CLIENT_MAIN:
            custom_test_code(argc,argv);
            break;
        case __CLIENT_RECV:
            recv_content(a._target_ip,a._port);
            break;
        case __CLIENT_SEND:
            if(_global_conf._CHECK_FILE && access(a._data,F_OK) != -1)
            {
                _sys_log("File found, loading \"%s\"");
                send_content(a._target_ip,a._port,a._data,__SEND_FILE);
            }
            else
                send_content(a._target_ip,a._port,a._data,__SEND_MESSAGE);
            break;
        case __CLIENT_CHAT:
            chat(a._target_ip,a._port);
            break;
        default:
            fprintf(stderr,"Invalid state (%i), exiting...\n",a._mode);
            break;
    };
    if(_global_conf._CLEANUP)
	    cleanup(_orig_mac,a._target_ip);
    
	return 0;
}

void configure(char * conf_path)
{
    if(conf_path != NULL && access(conf_path, F_OK) != -1)
    {
        FILE * f = fopen(conf_path,"r");
        
        fscanf(f,"%i\n%i\n%i\n%i\n%i\n%i",&(_global_conf._VERBOSE),&(_global_conf._FUNCLIST),&(_global_conf._SHUFFLE),&(_global_conf._ENCRYPT),&(_global_conf._CLEANUP),&(_global_conf._LOG_SYS));
        _sys_log("[Config Loaded]\n---------------\nPrintDebug: %i\nFuncList: %i\nShuffle: %i\nEncryption: %i\nCleanup: %i\nSysLogs: %i\n---------------\n",(_global_conf._VERBOSE),(_global_conf._FUNCLIST),(_global_conf._SHUFFLE),(_global_conf._ENCRYPT),(_global_conf._CLEANUP),(_global_conf._LOG_SYS));
    }
    else if(conf_path == NULL)
    {
        char * param_qs[6] = {"Print debug statements","Print function calls","Shuffle MAC addresses","Encrypt messages","Cleanup arp at end of session", "Log system messages to file"};
        int * params[6] = {&(_global_conf._VERBOSE),&(_global_conf._FUNCLIST),&(_global_conf._SHUFFLE),&(_global_conf._ENCRYPT),&(_global_conf._CLEANUP),&(_global_conf._LOG_SYS)};
        char _arg[8];
        int _bfr = -1;
        for(int i = 0; i < 6;)
        {
            _bfr = -1;
            printf("%s?:",param_qs[i]);
            fgets(_arg,8,stdin);
            _bfr = boolify(_arg);
            if(_bfr != -1){
                *(params[i]) = _bfr;
                i += 1;
            }
            else{
                printf("Invalid Response (valid responses are t/f,y/n,1/0)\n");
            }
        }
        if(_global_conf._LOG_SYS)
        {
            char log_path[128];
            memset(log_path,0,128);
            printf("File path for sys logs?:");
            fgets(log_path, 127,stdin);
            _global_conf._DB_OUTPUT_FD = open(log_path, O_CREAT | O_TRUNC);
            if(_global_conf._DB_OUTPUT_FD == -1)
            {
                printf("_DB_OUTPUT_FD failed to open, exiting...\n");
                perror("SYS-Fopen");
                exit(EXIT_FAILURE);
            }
        }
        
        int diff_out = -1;
        char out_buf[128];
        memset(out_buf,0,128);
        do{
            fgets(out_buf,8,stdin);
            printf("Send output to file?: ");
            diff_out = boolify(out_buf);
        }while(diff_out == -1);
        
        if(diff_out)
        {
            memset(out_buf,0,128);
            printf("Output file path?: ");
            fgets(out_buf,128,stdin);
            _global_conf._DB_OUTPUT_FD = open(out_buf, O_CREAT | O_TRUNC);
        }
        
    }
    
}

void parse_args(args * a, int argc, char ** argv)
{
    memset(a,0,sizeof(args));
    int mode_selected = 0, ip_present = 0,port_present = 0;
    for(int i = 1; i < argc; i++)
    {
        char * arg = argv[i];
        if(arg[0] == '-')
        {
            if(strlen(argv[i]) > 2)
                goto error;

            char c = arg[1];
            switch (c){
                case 'I':
                    if(i < (argc - 1))
                        strncpy((_global_conf._IFACE),argv[i + 1],min(strlen(argv[i + 1]),sizeof(_global_conf._IFACE)));
                    else
                        goto error;
                    i += 1;
                    break;
                case 'i':
                    if(i < (argc - 1) && !ip_present)
                    {
                        strncpy(a->_target_ip,argv[i + 1], min(strlen(argv[i + 1]),sizeof(a -> _target_ip)));
                        ip_present = 1;
                    }
                    else
                        goto error;
                    i += 1;
                    break;
                case 'o':
                    if(i < (argc - 1))
                    {
                        strncpy(a -> _out_path,argv[i + 1], min(strlen(argv[i + 1]),sizeof(a -> _out_path)));
                        _global_conf._DB_OUTPUT_FD = open(a -> _out_path, O_CREAT | O_TRUNC);
                        if(_global_conf._DB_OUTPUT_FD == -1)
                            perror("OUTPUT-Open");
                    }
                    else
                        goto error;
                    i += 1;
                    break;
                case 'l':
                    _global_conf._LOG_SYS = 1;
                    if(i < (argc - 1))
                        strncpy(a -> _log_path,argv[i + 1], min(strlen(argv[i + 1]),sizeof(a -> _log_path)));
                    else
                        goto error;
                    i += 1;
                    break;
                case 'c':
                    if(i < (argc - 1))
                        strncpy(a -> _conf_path,argv[i + 1], min(strlen(argv[i + 1]),sizeof(a -> _conf_path)));
                    else
                        goto error;
                    i += 1;
                    break;
                case 'C':
                    if(i < (argc - 1))
                        _global_conf._CLEANUP = atoi(argv[i + 1]);
                    else
                        goto error;
                    i += 1;
                    break;
                case 'p':
                    if(i < (argc - 1))
                    {
                        if(port_present)
                            goto error;
                        a -> _port = atoi(argv[i + 1]);
                        port_present = 1;
                    }
                    else
                        goto error;
                    i += 1;
                    break;
                case 'F':
                    if(i < (argc - 1))
                        _global_conf._FRAG_SIZE = atoi(argv[i + 1]);
                    else
                        goto error;
                    i += 1;
                    break;
                case 'e':
                    if(i < (argc - 1))
                        _global_conf._ENCRYPT = atoi(argv[i + 1]);
                    else
                        goto error;
                    i += 1;
                    break;
                case 'f':
                    _global_conf._CHECK_FILE = 1;
                    break;
                case 'h':
                    print_help();
                    exit(EXIT_SUCCESS);
                    break;
                case 'V':
                    print_version();
                    exit(EXIT_SUCCESS);
                    break;
                case 'v':
                    _global_conf._VERBOSE = 1;
                    break;
                case 'L':
                    _global_conf._FUNCLIST = 1;
                    break;
                case 'w':
                    //GOTO WIZARD
                    break;
                default:
                    goto error;
                    break;
            }
            continue;
        }
        else
        {
            
            if(!strncmp(argv[i],"rcv",4) || !strncmp(argv[i],"receive",8))
            {
                a -> _mode = __CLIENT_RECV;
                if(mode_selected)
                    goto error;
                mode_selected = 1;
            }
            else if(!strncmp(argv[i],"snd",4) || !strncmp(argv[i],"send",5))
            {
                a -> _mode = __CLIENT_SEND;
                if(mode_selected)
                    goto error;
                mode_selected = 1;
            }
            else if(!strncmp(argv[i],"cht",4) || !strncmp(argv[i],"chat",4))
            {
                a -> _mode = __CLIENT_CHAT;
                if(mode_selected)
                    goto error;
                mode_selected = 1;
            }
            else if(!strncmp(argv[i],"cli",4) || !strncmp(argv[i],"client",7))
            {
                a -> _mode = __CLIENT_MAIN;
                if(mode_selected)
                    goto error;
                mode_selected = 1;
            }
            else if(!strncmp(argv[i],"help",5))
            {
                print_help();
                exit(EXIT_SUCCESS);
            }
        }
    }
    if(_global_conf._LOG_SYS)
    {
        _global_conf._DB_OUTPUT_FD = open(a -> _log_path, O_CREAT | O_TRUNC);
        if(_global_conf._DB_OUTPUT_FD == -1)
        {
            perror("SYSLOG-Open");
            exit(EXIT_FAILURE);
        }
    }
    if(a -> _mode == __CLIENT_SEND)
    {
        a -> _data = argv[argc - 1];
    }
    if(!ip_present)
    {
        printf("Parsing Error: Target IP not found or malformed, please preface it with \"-i\", exiting...\n");
        exit(EXIT_FAILURE);
    }
    if(!port_present)
    {
        printf("Port not found, using default port(7755)\n");
        a -> _port = 7755;
    }
    _sys_log("Arguments Parsed\n-----------------\nTarget IP: %s\nTarget Port: %i\nMode: %i\nConfig Path: \"%.8s...\"\nOutput Path: \"%.8s...\"\nLog Path: \"%.8s...\"\n-----------------\n",a -> _target_ip, a -> _port, a ->_mode, a -> _conf_path, a -> _out_path, a -> _log_path);
    if(strlen(a -> _target_ip) < 7)
    {
        fprintf(stderr,"Parsing Error: missing or invalid IP, exiting...\n");
    }
    return;
    error:
        fprintf(stderr,"Invalid command line option, exiting...\n");
        exit(EXIT_FAILURE);
}

void wizard()
{
    enum {GENERAL, CONFIGURE, INFO, EXECUTE} state = GENERAL;
    int exec_option = 0;
    args a; memset(&a,0,sizeof(a));
    while(state != EXECUTE)
    {
        int option = 0;
        print_wizard_options();
        scanf("%i",&option);
        switch(state){
            case GENERAL:
            case CONFIGURE:
                printf("Configuring llss execution\n");
                configure(NULL);
                int save = -1;
                while(save = -1)
                {
                    printf("Would you like to save your configuration?: ");
                    char c_buf[8];memset(c_buf,0,8);fgets(c_buf,8,stdin);
                    save = boolify(c_buf);
                }
                if(save)
                {
                    printf("Saved file name?: ");
                    char savc_buf[128]; memset(savc_buf,0,128); fgets(savc_buf,127,stdin);
                    FILE * new_conf = fopen(savc_buf,"r");
                    memset(savc_buf,128,0);

                    sprintf(savc_buf,"%i\n%i\n%i\n%i\n%i\n%i\n%i\n",_global_conf._VERBOSE,_global_conf._FUNCLIST,_global_conf._SHUFFLE,_global_conf._ENCRYPT,_global_conf._CLEANUP,_global_conf._LOG_SYS,_global_conf._CHECK_FILE);
                    if (fwrite(savc_buf,strlen(savc_buf),1,new_conf) == -1)
                    {
                        perror("sconf-fwrite");
                        exit(EXIT_FAILURE);
                    }
                }
                state = GENERAL;  
            case INFO:
                printf("Target IP?: ");
                fgets(a._target_ip,16,stdin);
                
                printf("Target Port?: ");
                char p_buf[4]; fgets(p_buf,4,stdin);
                a._port = strtol(p_buf,NULL,10);
                if(a._port == __LONG_MAX__)
                {
                    printf("Port input invalid, using default port (%i).\n If needed, reconfigure.\n",__DEFAULT_PORT);
                }
                state = GENERAL;
                break;
            case EXECUTE:
                break;
            default:
                fprintf(stderr,"Invalid state, exiting...");
                exit(EXIT_FAILURE);
                break;
        }
    }

}
void print_wizard_options()
{
	printf("1. Get Current Mac Address\n");
	printf("2. Send message\n");
	printf("3. Send file\n");
    printf("4. Receive message or file\n");
    printf("5. Configure llss (temporary)\n");
    printf("6. Load config file");

    //If you happen to be compiling/editing this yourself, hello :)
    printf("0. Custom Test Code"); 
}

void print_help(){
    print_version();
    printf("usage: llss <snd,rcv,cht,cli> <ip> <port> [args] [data]\n");
    printf("\n--Notes on standard usage--\n * All arguments can go in any order so long as their required information follows it.\n");
    printf(" * Argument parsing is not perfect and may sometimes break, but most standard edge cases should be covered.\n");
    printf(" * I am a college student single-handedly working on this, there are likely security flaws in this program.\n  That doesnt mean that it is not a good security tool, it just means it is in progress.\n");
    printf("\nIf you find a bug/vulnerability, please let me know! by emailing thomasquig.dev@gmail.com or through GitHub\n");
    printf("\n--------------------------------------llss arguments--------------------------------------\n");
    printf("-c <path>\t Path to the configuration file. This OVERRIDES any other configuration settings set in the arguments\n");
    printf("-C <1/0>\t Enable mac address cleanup at the end of program execution. This is off by default.\n");
    printf("-e\t\t Enable or disable encryption. Enabled by default\n");
    printf("-f\t\t Load the file at path [data] and send it. Selcting this enables _CHECK_FILE in config.\n");
    printf("-F <size>\t Fragmentation size of the sender's data. Default is 1024\n");
    printf("-i <ip>\t\t The IP of the target, **preface <ip> with -i**\n");
    printf("-I <iface>\t The interface to shuffle MAC addresses on. Ensure that the network you are on allows for Static IP's and no DHCP\n");
    printf("-l <path>\t Log file, all _sys_log calls will be sent to this file\n");
    printf("-o <path>\t Output file, all non _sys_log call output will be sent to the output file at <path>\n");
    printf("-p <port>\t The port you will be sending to / receiving from. Be aware that this runs atoi(3), so any noninteger is undefined behaviour.\n");
    printf("-h\t\t Display this help message.\n");
    printf("-V\t\t Print version information\n");
    printf("-v\t\t Verbose mode, enabling this will output all debu _sys_log messages. This is required for -l to have any content.\n");
    printf("------------------------------------------------------------------------------------------\n\n");
    printf("--Examples--\n\nrunllsss send -i 192.168.0.2 -p 3333 \"Hello World!\"\n\tSends \"Hello World!\" to 192.168.0.2:3333 in 1 packet\n\n");
    printf("runllss send -i 192.168.0.2 -p 3333 -F 6 \"Hello World!\"\n\tSends \"Hello \" followed by \"World!\" (2 packets)\n\n");
    printf("runllss send -e 0 -c c1.conf -l log.txt -v -i 192.168.0.2 -p 3333 \"Hello World!\"\n\tSends \"Hello World!\" in 1 packet, verbose, all logs go to log.txt, configure with c1.conf\n\n");
    printf("runllss receive -i 192.168.0.3 -p 3333 -o out.txt\n\tReceives from 192.168.0.3:3333 and saves the file to \"out.txt\"\n");
}

void print_version(){
    printf("llss Version %i.%i.%i, by Thomas Quig\n",_lvn,_mvn,_rvn);
}

int custom_test_code(int argc, char ** argv)
{
	
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
            fprintf(stderr,"Coult not open file \"%s\"", arg);
            perror(" ");
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
                if(_global_conf._ENCRYPT)
                {
                    char cipher[content_size + 16 - (content_size % 16)];
                    int cypher_len = encrypt(content,content_size,conn -> secret, conn -> secret + 16,cipher);
                    send_loop(conn,cipher,cypher_len);
                }
                else{
                    send_loop(conn,content,content_size);
                }
                send_loop(conn,content,content_size);
                tot_bytes_sent += content_size;
            }
        }
    }
    else if(mode == __SEND_MESSAGE)
    {
        
        if(_global_conf._ENCRYPT)
        {
            char cipher[strlen(arg) + 16 - (strlen(arg) % 16)];
            int cypher_len = encrypt(arg,strlen(arg),conn -> secret, conn -> secret + 16,cipher);
            send_loop(conn,cipher,cypher_len);
        }
        else{
            send_loop(conn,arg,strlen(arg));
        }
        
    }
    else
    {
        printf("How\n");
        return;
    }
    
}

size_t send_loop(connection * conn, char * content, size_t content_size){
    ssize_t data_size = ds_exchange(conn);
    size_t tot_sent = 0;
    while(tot_sent < content_size)
    {
        char * next_macs = get_next_macs(__CLIENT_SEND);

        ssize_t tmp_sent = 0;
        size_t to_send = min(content_size - tot_sent,_global_conf._FRAG_SIZE);

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
    s_send(conn,"[ENDMSG]",min(_global_conf._FRAG_SIZE,8));
    char endbuf[12];
    printf("Received final packet %ld:\"%.12s\", process complete, cleaning up.\n",s_recv(conn,endbuf,12),endbuf);
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
    char rcv_buf[_global_conf._FRAG_SIZE];
    memset(rcv_buf,0,_global_conf._FRAG_SIZE);
    ssize_t data_size = ds_exchange(conn);
    if(data_size < 0)
    {
        perror("ds-exchange");
        return 0;
    }
    ssize_t bytes_rcvd;
    ssize_t bytes_rspd;
    ssize_t tot_rcvd;
    int rcv_data = 1;
    while (rcv_data)
    {
        char * next_macs = get_next_macs(__CLIENT_RECV);

        _sys_log("Waiting on data...\n");
        bytes_rcvd = s_recv(conn,rcv_buf + tot_rcvd,_global_conf._FRAG_SIZE);
        if(bytes_rcvd == -1){
            perror("s_recv");
        }
        tot_rcvd += bytes_rcvd;
        _sys_log("[RCVD] Received %d bytes\n",bytes_rcvd);
        advance_mac(conn,next_macs,__ADV_OTHR);
        
        //rcv_data = strncmp(rcv_buf,"[ENDMSG]",min(_global_conf._FRAG_SIZE,8));
        //if(rcv_data)
        char resp_buf[12];
        memset(resp_buf,0,12);
        sprintf(resp_buf,"ACK:%.4x",*((int *)rcv_buf));
        bytes_rspd = s_send(conn, resp_buf,strlen(resp_buf));
        
        advance_mac(conn,next_macs,__ADV_SELF);
    }
    if(_global_conf._ENCRYPT)
    {
        char plaintext[conn -> data_size];
        int plaintext_size = decrypt(rcv_buf,tot_rcvd,conn -> secret, conn -> secret + 16, plaintext);
        write(_global_conf._OUTPUT_FD,plaintext,plaintext_size);
    }
    else
    {
        write(_global_conf._OUTPUT_FD,rcv_buf,tot_rcvd);
    }
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