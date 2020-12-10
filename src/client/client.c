#include "client.h"

static char s_target_ip[16];
static char s_orig_mac[6];
int _lvn = 1,_mvn = 2,_rvn = 71;
void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        fprintf(stderr,"\nReceived user interrupt via ^C, safely exiting...\n");
        //set_mac(_global_conf._IFACE,_orig_mac);
        char cmd[128];
        memset(cmd,0,128);
        
        sprintf(cmd,"arp -d %s",s_target_ip);
        system(cmd);
        fprintf(stderr,"Recover original mac address with \"ping -I %s %s\"\n",_global_conf._IFACE,s_target_ip);

        exit(0);
    }
}

int client_main(int argc, char ** argv)
{	
    //Signal Handler for SIGINT
    signal(SIGINT,sig_handler);

    //Initial configuration, is overwritten by parse-args and any future configs.
    configure("./llss.conf");

    args a;memset(&a,0,sizeof(a));
    parse_args(&a,argc,argv); //Parse args will exit if -w is called
    if(strlen(a._conf_path) > 0){
        printf("Configuring using %s",a._conf_path);
        configure(a._conf_path);
    }

    memset(s_target_ip,0,16);//This way its all null
    strncpy(s_target_ip,a._target_ip,min(strlen(a._target_ip),15));
    memcpy(s_orig_mac,get_mac(_global_conf._IFACE),12);

    if(a._mode == __CLIENT_MAIN){
        custom_test_code(argc,argv);
    }
    else{
        execute(a);
    }

    if(_global_conf._CLEANUP)
	    cleanup(s_orig_mac,a._target_ip);
    
	return 0;
}

void execute(args a)
{
    _sys_log("Arguments Parsed\n-----------------\nTarget IP: %s\nTarget Port: %i\nMode: %i\nConfig Path: \"%.8s...\"\nOutput Path: \"%.8s...\"\nLog Path: \"%.8s...\"\n-----------------\n",a._target_ip, a._port, a._mode, a._conf_path, a._out_path, a._log_path);
    switch(a._mode){
        case __CLIENT_RECV:
            recv_content(a._target_ip,a._port);
            break;
        case __CLIENT_SEND:
            if(_global_conf._CHECK_FILE && access(a._data,F_OK) != -1)
            {
                _sys_log("File found, loading \"%32s...\"\n",a._data);
                send_content(a._target_ip,a._port,a._data,__SEND_FILE);
            }
            else                                                
                send_content(a._target_ip,a._port,a._data,__SEND_MESSAGE);
            break;
        default:
            fprintf(stderr,"Invalid state (%i), exiting...\n",a._mode);
            break;
    };
}

void configure(char * conf_path)
{
    if(conf_path != NULL && access(conf_path, F_OK) != -1)
    {
        FILE * f = fopen(conf_path,"r");
        
        fscanf(f,"%i\n%i\n%i\n%i\n%i\n%i\n%i\n%i\n%i",&(_global_conf._VERBOSE),&(_global_conf._FUNCLIST),&(_global_conf._SHUFFLE),&(_global_conf._ENCRYPT),&(_global_conf._CLEANUP),&(_global_conf._LOG_SYS),&(_global_conf._CHECK_FILE),&(_global_conf._FRAG_SIZE),&(_global_conf._CSTMSEED));
        _sys_log("[Config Loaded]\n---------------\nPrintDebug: %i\nFuncList: %i\nShuffle: %i\nEncryption: %i\nCleanup: %i\nSysLogs: %i\nCheckFile: %i\nFragSize: %i\nCustomSeed: %i\n---------------\n",_global_conf._VERBOSE,_global_conf._FUNCLIST,_global_conf._SHUFFLE,_global_conf._ENCRYPT,_global_conf._CLEANUP,_global_conf._LOG_SYS,_global_conf._CHECK_FILE,_global_conf._FRAG_SIZE,_global_conf._CSTMSEED);
    }
    else if(conf_path == NULL)
    {
        //Advanced settings (Frag size and custom seed) are left for loading from a file (you cant do it manually).
        char * param_qs[7] = {"Verbose mode (print debug statements)","Print function calls","Shuffle MAC addresses","Encrypt messages","Cleanup arp at end of session", "Log system (debug/funccall) messages to file", "Check for file when sending message"};
        int * params[7] = {&(_global_conf._VERBOSE),&(_global_conf._FUNCLIST),&(_global_conf._SHUFFLE),&(_global_conf._ENCRYPT),&(_global_conf._CLEANUP),&(_global_conf._LOG_SYS),&(_global_conf._CHECK_FILE)};
        char _arg[8];
        int _bfr = -1;
        for(int i = 0; i < 7;)
        {
            _bfr = -1;
            printf("%s?: ",param_qs[i]);
            scanf("%8s",_arg);
            _bfr = boolify(_arg);
            if(_bfr != -1){
                *(params[i]) = _bfr;
                i += 1;
            }
            else{
                printf("Invalid Response, no change (valid responses are t/f,y/n,1/0)\n");
            }
        }
        if(_global_conf._LOG_SYS)
        {
            char log_path[128];
            memset(log_path,0,128);
            printf("File path for sys logs?: ");
            scanf("%127s",log_path);
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
            printf("Send output to file?: ");
            scanf("%8s",out_buf);
            diff_out = boolify(out_buf);
            if(diff_out == -1)
                printf("Invalid Response (valid responses are t/f,y/n,1/0)\n");

        }while(diff_out == -1);
        
        if(diff_out)
        {
            memset(out_buf,0,128);
            printf("Output file path?: ");
            scanf("%127s",out_buf);
            _global_conf._OUTPUT_FD = open(out_buf, O_CREAT | O_TRUNC);
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
                        _global_conf._OUTPUT_FD = open(a -> _out_path, O_CREAT | O_TRUNC | O_RDWR);
                        _sys_log("Output File on FD %i\n",_global_conf._OUTPUT_FD);
                        if(_global_conf._OUTPUT_FD == -1)
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
                        _global_conf._DB_OUTPUT_FD = open(a -> _log_path, O_CREAT | O_TRUNC | O_RDWR);
                        if(_global_conf._DB_OUTPUT_FD == -1)
                            perror("OUTPUT-Open");
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
                    _sys_log("Verbose has been enabled\n");
                    break;
                case 'L':
                    _global_conf._FUNCLIST = 1;
                    break;
                case 's':
                    if(i < (argc - 1))
                        _global_conf._SHUFFLE = atoi(argv[i + 1]);
                    else
                        goto error;
                    i += 1;
                    break;
                case 'w':
                    wizard();
                    exit(EXIT_SUCCESS);
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
        _global_conf._DB_OUTPUT_FD = open(a -> _log_path, O_CREAT | O_TRUNC | O_RDWR);
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
    if(!ip_present){
        fprintf(stderr,"Parsing Error: Target IP not found or malformed, please preface it with \"-i\", exiting...\n");
        exit(EXIT_FAILURE);
    }
    if(!port_present){
        printf("Port not found, using default port(7755)\n");
        a -> _port = 7755;
    }
    if(!mode_selected){
        fprintf(stderr,"Parsing Error: Mode not selected, exiting...\n");
        exit(EXIT_FAILURE);
    }
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
    char data[1024]; memset(data,0,1024);
    print_logo();
    print_version();
    printf("Booting wizard");
    for(int i = 0; i < 24; i++)
    {
        usleep(50000);
        printf(".");
        fflush(stdout);
    }
    enum {GENERAL, CONFIGURE, INFO, EXECUTE, FINISHED} state = GENERAL;
    int mode_selected = 0,ip_present = 0, port_present = 0;
    int skip_delay = 0;

    args a; memset(&a,0,sizeof(a));
    while(state != FINISHED)
    {
        char opt_buf[5];
        switch(state){
            case GENERAL:
                if(!skip_delay)
                    usleep(1500000);
                else
                    usleep(250000);
                print_wizard_options();
                memset(opt_buf,0,5);
                scanf("%4s",opt_buf); //TODO USE STRTOL TO GET ERRORS
                if(toupper(opt_buf[0]) == 'F')
                {
                    skip_delay = !skip_delay;
                    break;
                }
                char * endptr = NULL;
                int option = strtol(opt_buf,&endptr,10);
                if(endptr == opt_buf)
                    option == -1;
                fflush(stdin);
                switch(option){
                    case 1:
                        a._mode = __CLIENT_SEND;
                        _global_conf._CHECK_FILE = 0;
                        printf("You are now set to send a message\n");
                        mode_selected = 1;
                        break;
                    case 2:
                        a._mode = __CLIENT_SEND;
                        _global_conf._CHECK_FILE = 1;
                        printf("You are now set to send a file \n");
                        mode_selected = 1;
                        break;
                    case 3:
                        a._mode = __CLIENT_RECV;
                        printf("You are now configured to receive a file \n");
                        mode_selected = 1;
                        break;
                    case 4:
                        state = CONFIGURE;
                        break;
                    case 5:
                        save_config(NULL);
                        printf("Successfully saved current configuration to file \n");
                        break;
                    case 6:
                        state = INFO;
                        break;
                    case 7:
                        print_help();
                        printf("Press Enter to Continue");
                        while( getchar() != '\n' );
                        break;
                    case 8:
                        a._mode = __CLIENT_MAIN;
                        printf("Configured to run custom code \n");
                        break;
                    case 9:
                        if(!ip_present){
                            fprintf(stderr,"Error: Target IP not found or malformed, please enter it... \n");
                            break;
                        }
                        if(!port_present){
                            printf("Port not found, using default port(7755)\n");
                            a._port = 7755;
                        }
                        if(!mode_selected){
                            fprintf(stderr,"Error: Mode not selected, please select it...\n");
                            break;
                        }
                        if(a._mode == __CLIENT_SEND)
                        {
                            char sendbuf[1024];memset(sendbuf,0,1024);
                            printf("You are sending a message, more information required\n---Enter your message below---\n");
                            scanf("%1023s",sendbuf);
                            printf("------------------------------\n");
                            a._data = sendbuf;
                        }
                        state = EXECUTE;
                        break;
                    case 0:
                        if(option == 0)
                        {
                            char confbuf[8]; memset(confbuf,0,8); printf("Are you sure (y/n)? "); scanf("%8s",confbuf);
                            if(boolify(confbuf)){
                                printf("Exiting...\n");
                                exit(EXIT_SUCCESS);
                            }
                        }
                    default:
                        printf("Invalid option, select from above.");
                        break;
                }
                break;
            case CONFIGURE:
                printf("Configuring llss execution\n");
                configure(NULL);
                int save = -1;
                while(save == -1)
                {
                    printf("Would you like to save your configuration?: ");
                    char c_buf[8];memset(c_buf,0,8);scanf("%7s",c_buf);
                    save = boolify(c_buf);
                }
                if(save)
                {
                    printf("Saved file name?: ");
                    char savc_buf[128]; memset(savc_buf,0,128); scanf("%127s",savc_buf);
                    FILE * new_conf = fopen(savc_buf,"w");
                    memset(savc_buf,128,0);

                    sprintf(savc_buf,"%i\n%i\n%i\n%i\n%i\n%i\n%i\n",_global_conf._VERBOSE,_global_conf._FUNCLIST,_global_conf._SHUFFLE,_global_conf._ENCRYPT,_global_conf._CLEANUP,_global_conf._LOG_SYS,_global_conf._CHECK_FILE);
                    if (fwrite(savc_buf,strlen(savc_buf),1,new_conf) == -1)
                    {
                        perror("sconf-fwrite");
                        exit(EXIT_FAILURE);
                    }
                }
                state = GENERAL;  
                break;
            case INFO: //THIS IS 100& not working
                printf("Target IP?: ");
                scanf("%16s",a._target_ip);
                printf("Target Port?: ");
                scanf("%i",&a._port);
                if(a._port == 0)
                {
                    printf("Port input invalid, using default port (%i).\n If needed, reconfigure.\n",__DEFAULT_PORT);
                    a._port = __DEFAULT_PORT;
                }
                ip_present = 1, port_present = 1;
                state = GENERAL;
                break;
            case EXECUTE:
                printf("Executing llss...\n");
                execute(a);

                if(_global_conf._CLEANUP)
                    cleanup(s_orig_mac,s_target_ip);
                char confbuf[8]; memset(confbuf,0,8); printf("Continue (y/n)? "); scanf("%8s",confbuf);
                if(boolify(confbuf) == 0){
                    printf("Exiting...\n");
                    state = FINISHED;
                }
                break;
            default:
                fprintf(stderr,"Invalid state, exiting...");
                exit(EXIT_FAILURE);
                break;
        }
    }
    printf("Thanks for using the llss Wizard! See you again soon :)\n");
}

void print_logo()
{
    char logo[] = "\n\n\n"
    "██╗     ██╗     ███████╗███████╗\n"
    "██║     ██║     ██╔════╝██╔════╝\n"
    "██║     ██║     ███████╗███████╗\n"
    "██║     ██║     ╚════██║╚════██║\n"
    "███████╗███████╗███████║███████║\n"
    "╚══════╝╚══════╝╚══════╝╚══════╝\n\n\n";
    printf("%s",logo);
}

void print_wizard_options()
{
    printf("\e[1;1H\e[2J");
    print_version();

	printf("1. Send message\n");
	printf("2. Send file\n");
    printf("3. Receive message or file\n");
    printf("4. Configure llss (settings)\n");
    printf("5. Save configration\n");
    printf("6. Set critical information (ip, port)\n");
    printf("7. Command line help\n");
    printf("8. Custom Test Code\n"); 
    printf("9. Execute\n"); //If you happen to be compiling/editing this yourself, hello :)
    printf("0. Exit\n\n");
    printf("F. Toggle fast mode (wizard only)\n");
    printf("Choice: ");
}

void print_help(){
    print_version();
    printf("usage: llss <snd,rcv,cht,cli> <ip> <port> [args] [data]\n");
    printf("\n--Notes on standard usage--\n * All arguments can go in any order so long as their required information follows it.\n");
    printf(" * Argument parsing is not perfect and may sometimes break, but most standard edge cases should be covered.\n");
    printf(" * I am a college student single-handedly working on this, there are likely security flaws in this program.\n  That doesnt mean that it is not a good security tool, it just means it is in progress.\n");
    printf(" * Help, Version, and Wizard will display depending on which is first in argv[], so if -w is first you get wizard, if -v version etc.\n");
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
    printf("-w\t\t Enter the wizard, OVERRIDES ANY INFORMATION / CONFIGURATION SETTINGS\n");
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
	//Put your custom test code here
	return 0;
}

void send_content(char * ip, int port, char * arg, int mode)
{
    connection * conn = establish_connection(ip,port,__CLIENT_SEND);
    //Initial setup has a 1/100th second delay so receiver can establish an open port.
    usleep(100000);

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
            _sys_log("Sending file \"%s\" to %s\n",arg,conn -> ip);
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
                    char cipher[content_size + (16 - (content_size % 16))];
                    int cypher_len = encrypt(content,content_size,conn -> secret, conn -> secret + 16,cipher);
                    send_loop(conn,cipher,cypher_len);
                }
                else{
                    send_loop(conn,content,content_size);
                }
                tot_bytes_sent += content_size;
            }
        }
    }
    else if(mode == __SEND_MESSAGE)
    {
        
        if(_global_conf._ENCRYPT)
        {
            char cipher[strlen(arg) + (16 - (strlen(arg) % 16))];
            int cypher_len = encrypt(arg,strlen(arg),conn -> secret, conn -> secret + 16,cipher);
            send_loop(conn,cipher,cypher_len);
        }
        else{
            send_loop(conn,arg,strlen(arg));
        }
    }
    else
    {
        fprintf(stderr,"Illegal state, exiting...\n");
        return;
    }
    s_send(conn,"[ENDMSG]",min(_global_conf._FRAG_SIZE,8));
    char endbuf[13];memset(endbuf,0,13);
    if(s_recv(conn,endbuf,12) == -1){
        perror("s_recv-end");
        return;       
    }
    _sys_log("Received final packet\"%s\", process complete.\n",endbuf,endbuf);
}

size_t send_loop(connection * conn, char * content, size_t content_size){
    _sys_log("send_loop(%p,%.8s,%du)\n",conn,content,content_size);

    size_t tot_sent = 0;
    int iter = 0;
    char next_macs[12]; memset(next_macs,0,12);
    while(tot_sent < content_size)
    {
        _sys_log("\n\n[Iter %i, %u/%u]\n",iter,tot_sent,content_size);
        get_next_macs(__CLIENT_SEND,next_macs);

        ssize_t tmp_sent = 0;
        size_t to_send = min(content_size - tot_sent,_global_conf._FRAG_SIZE);
        _sys_log("Offset %d\n",tot_sent);
        tmp_sent = s_send(conn,content + tot_sent, to_send);
        if(tmp_sent == -1){
            perror("s_send");
            break;
        }
        tot_sent += tmp_sent;
        _sys_log("[SND]%d Bytes sent to %s\n\n",tmp_sent,conn -> ip);
        advance_mac(conn,next_macs,__ADV_SELF);

        int acked = 0;
        while(!(acked))
        {
            char response[12];
            ssize_t rf_resp = s_recv(conn,response,12);
            response[rf_resp] = '\0';
            _sys_log("[RSP] Response receieved...\n\"%s\"\n[End Response]\n\n",response);
            if(!strncmp(response,"ACK",3))
                acked = 1;
        }
        advance_mac(conn,next_macs,__ADV_OTHR);
        iter++;
    }
}

void recv_content(char * ip, int port)
{
    //The IP here is neccesary for ACK sending. It originlly was not (fun fact).
    connection * conn = establish_connection(ip,port,__CLIENT_RECV);
    int rl_ret = recv_loop(conn);
	if(rl_ret == -1)
        fprintf(stderr, "Receive failed at %d bytes, exiting\n",-rl_ret);
	
    //Chances are conn -> fd is already closed, but this makes sure of it.
    close(conn -> fd); 
	free(conn);
}

int recv_loop(connection * conn)
{
    _sys_log("recv_loop(%p)\n",conn);
    ssize_t tot_rcvd = 0;
    int rcv_data = 1;
    while (rcv_data)
    {
        char cur_large_buf[__MAX_BUFFER_SIZE];memset(cur_large_buf,0,__MAX_BUFFER_SIZE);
        ssize_t clb_rcvd = 0;
        int iter = 0;
        int fin_on_bborder = 0;
        do{
            _sys_log("\n\n[Iter %i]\n",iter);
            
            //Initialize vars within loop
            ssize_t bytes_rcvd = 0, bytes_rspd = 0;
            char rcv_buf[_global_conf._FRAG_SIZE];memset(rcv_buf,0,_global_conf._FRAG_SIZE);
            char resp_buf[12]; memset(resp_buf,0,12);

            //Get next mac addresses for shuffling. Should shuffling be disabled, this is unneccesary.
            char next_macs[12]; memset(next_macs,0,12);
            get_next_macs(__CLIENT_RECV,next_macs);
            
            //Receieve the current fragment of data.
            bytes_rcvd = s_recv(conn,rcv_buf,_global_conf._FRAG_SIZE);
            _sys_log("recv_loop(): Received %d bytes\n",bytes_rcvd);
            if(bytes_rcvd == -1){ 
                perror("s_recv"); return -tot_rcvd;
            }
            
            //If Reliability is set to asynchronous, advance the sender's MAC address
            if(_global_conf._RELIABLE_MODE == __RELIABLE_ASYNC)
                advance_mac(conn,next_macs,__ADV_OTHR);
                
            memset(resp_buf,0,12);
            sprintf(resp_buf,"ACK:%.4x",*((int *)rcv_buf));
            if(bytes_rspd = s_send(conn, resp_buf,strlen(resp_buf)) == -1){
                perror("recv-rspd"); return -tot_rcvd;
            }

            //If the reliability mode is set to synchronous, advance the other host's MAC
            //Otherwise, just advance receiver (your) MAC.
            if(_global_conf._RELIABLE_MODE == __RELIABLE_SYNC)
                advance_mac(conn,next_macs,__ADV_OTHR);
            advance_mac(conn,next_macs,__ADV_SELF);
            
            //Advance iteration counter
            iter++;

            //Check to make sure the message was not the endmsg.
            //YES THIS DOES MEAN THAT IF YOUR MESSAGE IS "[ENDMSG]" your code stops processing.
            rcv_data = strncmp(rcv_buf,"[ENDMSG]",min(_global_conf._FRAG_SIZE,8));
            if(rcv_data != 0){
                memcpy(cur_large_buf + clb_rcvd,rcv_buf,bytes_rcvd);
                clb_rcvd += bytes_rcvd;
            }
            else{
                //Leave the inner loop if you receive ENDMSG
                _sys_log("recv_loop(): received end message (%i)\n",clb_rcvd);
                if(clb_rcvd == 0){
                    fin_on_bborder = 1;
                    _sys_log("recv_loop(): Finished on mod 0 of _MAX_BUF_SIZE\n");
                }
                break;
            }
        } while(clb_rcvd < __MAX_BUFFER_SIZE);
        //This implies that if _FRAG_SIZE > __MAX_BUFFER_SIZE this immediately breaks. 
        //So don't do it, and don't tell me I didnt warn you.


        //Add your current large buffer rcvd (min(__MAX_BUFFER_SIZE))
        tot_rcvd += clb_rcvd;

        //Check if decryption is needed additionally ensure that the final message sent wasnt on the border of a __MAX_BUF_SIZE
        if(_global_conf._ENCRYPT && !fin_on_bborder)
        {
            //Plaintext should be no larger than the tot_rcvd, as tot_rcvd is ciphertext size.
            char plaintext[tot_rcvd];
            
            int plaintext_size = decrypt(cur_large_buf,clb_rcvd,conn -> secret, conn -> secret + 16, plaintext);
            ssize_t w_ret = write(_global_conf._OUTPUT_FD,plaintext,plaintext_size);
            _sys_log("write(%i,\"%.8s...\",%u) = %d\n",_global_conf._OUTPUT_FD,plaintext,plaintext_size,w_ret);
            if(w_ret == -1){
                perror("wret_write:"); return -tot_rcvd;
            }
        }
        else if(!fin_on_bborder)
        {
            //Writing to _OUTPUT_FD instead of STDOUT allows for file writes using the exact same methods.
            //That is why I went through the painstakingly long process of allowing it. It's clean.
            ssize_t w_ret = write(_global_conf._OUTPUT_FD,cur_large_buf,clb_rcvd);
            _sys_log("write(%i,\"%.8s...\",%u) = %d\n",_global_conf._OUTPUT_FD,cur_large_buf,tot_rcvd,w_ret);
            if(w_ret == -1){
                perror("wret_write:"); return -tot_rcvd;
            }
        } 
    }
    return tot_rcvd;
}

void cleanup(char * orig_mac, char * ip)
{
    _sys_log("cleanup(%s,%s)\n",format_mac(orig_mac),ip);
	set_mac(_global_conf._IFACE,orig_mac);
	char cmd[64];
	sprintf(cmd,"arp -d %s",ip);
	system(cmd);
}