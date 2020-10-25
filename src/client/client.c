#include "client.h"
#define nl newline

void print_wizard_options()
{
	printf("1. Get Current Mac Address");
	printf("2. Change Mac Address (User Input)");
	printf("3. Change Mac Address (Conf File, Random)");
	printf("4. Change Mac Address (Conf File, Pregen)");
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

void send_content_loop(int sockfd, struct sockaddr_in servaddr)
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
		ssize_t bytes_sent = sendto(sockfd, (const char *)buf, size, 
			MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
				sizeof(servaddr)); 
		printf("%zd bytes sent.\n",bytes_sent); 
			
		n = recvfrom(sockfd, (char *)buf, sizeof(buf),  
					MSG_WAITALL, (struct sockaddr *) &servaddr, 
					(socklen_t * )&len); 
		buf[n] = '\0'; 
		printf("Server Response: %s\n", buf); 
		char cont;
		printf("Continue? (y/n):");
		scanf("%c",&cont);
		printf("Char: %c\n",cont);
		if(cont != 'y' && cont != 'Y')
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
	close(sockfd); 
}

void send_user_content(char * ip,int port)
{
	int sockfd; 
    struct sockaddr_in     servaddr; 
  
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port); 
    servaddr.sin_addr.s_addr = inet_addr(ip); 
	send_content_loop(sockfd,servaddr);
}


int client_main(int argc, char ** argv)
{	
	printf("Original Mac: ");
	print_mac("wlan0");nl();
	send_user_content(argv[2],atoi(argv[3]));

	return 0;
}