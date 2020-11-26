#include "server.h"

int server_main(int argc, char ** argv)
{
    printf("DEPRECATED, DONT RUN THIS CODE\n");
    /**
	int sockfd; 
    char buffer[1024]; 
    char *hello = "Hello from server"; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(atoi(argv[2])); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        
        exit(EXIT_FAILURE); // Bonk
    } 
      
    int len, n; 
  
    len = sizeof(cliaddr);  //len is value/resuslt 
	while(strncmp(buffer,"_KILL",6))
	{
		n = recvfrom(sockfd, (char *)buffer, 1024,  
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                (socklen_t *)&len); 
		buffer[n] = '\0'; 
		printf("Client : %s\n", buffer); 
		sendto(sockfd, (const char *)hello, strlen(hello),  
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
				len); 
		printf("Response sent.\n"); 
	} 
    return 0;**/
}