#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

#define PORT 		53053
#define MAXLINE 	1024
#define SERVER_IP 	"127.0.0.10"

int main() { 
	int sockfd; 
	char buffer[MAXLINE]; 
	char *hello = "Hello from client"; 
	struct sockaddr_in	 servaddr;
	char input[64];

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int n, len; 

	while(1){
		scanf("%s", input);
		sendto(sockfd, input, strlen(input),
			   MSG_CONFIRM, (const struct sockaddr *)&servaddr,
			   sizeof(servaddr));
		printf("Sent message to server.\n"); 
		
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
					 MSG_WAITALL, (struct sockaddr *) &servaddr, 
					 &len); 
		buffer[n] = '\0'; 
		printf("Server: %s\n", buffer); 
	}

	close(sockfd); 
	return 0; 
} 

