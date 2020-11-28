#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#define PORT 4030

int main()
{
	
	int sockfd, rc;
	socklen_t len;
	char buffer[1500];
	char *rMsg = "Hello from the other side";
	
	struct sockaddr_in myaddr;
		
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&myaddr, 0, sizeof(struct sockaddr_in));

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(PORT);
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sockfd, (struct sockaddr *)&myaddr, sizeof(struct sockaddr_in));

	while(1)
	{
		len = sizeof(struct sockaddr_in);

		strcpy(buffer, rMsg);

		// non-negative digit by success
		rc = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &myaddr, &len);

		// if was somethin got => send response
		if (rc != -1)
		{
			sendto(sockfd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &myaddr, len);		

		}

	}


	exit(0);


}


