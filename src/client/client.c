#include "client.h"
#define nl newline

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
	send_user_content(argv[2],atoi(argv[3]));
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
		change_mac("wlan0",_newmac); 
		print_mac("wlan0"); nl();
		system("ifconfig | grep -A 5 'wlan0'");
		printf("Exit? (y/n): ");
	}
	while(getc(stdin) != 'y');
}

void user_send_content_loop(connection * conn)
{
	
	uint8_t _currmac[6] = {0x00,0x00,0x00,0x00,0x00,0x11};
	while(1)
	{
		char buf[1024];
		memset(buf,0,1024);
		printf("==========Message[1024]==========\n");fflush(stdout);
		size_t size = read(STDIN_FILENO,buf,1024);
		printf("===========End Message===========\n");fflush(stdout);
		
		int n, len; 
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
		if(cont[0] != 'Y')
		{
			break;
		}
		else
		{
			_currmac[5] += 17;
			change_mac("wlan0",_currmac);
			continue;
		}
		
	}
	close(conn -> fd); 
	free(conn);
}

void send_user_content(char * ip,int port)
{
	connection * conn = establish_connection(ip,port);
	user_send_content_loop(conn);
}


int client_main(int argc, char ** argv)
{	
	printf("Original Mac: ");
	print_mac("wlan0");nl();
	custom_test_code(argc,argv);

	return 0;
}