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
	
	int i, sockfd, snd, n_sel, rc, nfds;
	socklen_t len;
	char buffer[1500];
	char *sMsg = "Hi?.. Is anybody home? ";
	
	fd_set readfds, readfds_c;
	
	struct timeval timeout;
	struct sockaddr_in myaddr;
		
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&myaddr, 0, sizeof(struct sockaddr_in));

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(PORT);
	myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");


	// clear the set ahead of time
	FD_ZERO(&readfds);

	// add our descriptors to the set
	FD_SET(sockfd, &readfds);

	// since we got sockfd second, it's the "greater", so we use that for
	// the n param in select()
	nfds = sockfd + 1;

	readfds_c = readfds;	


	len = sizeof(struct sockaddr_in);

	strcpy(buffer, sMsg);

	// On success, these calls return the number of characters sent. On error, -1 is returned, 
        // and errno is set appropriately. 
	snd  = sendto(sockfd, buffer, strlen(buffer)+1, 0, (struct sockaddr *) &myaddr, len);
	
	// if was successfuly sent => wait for response
	if (snd != -1)
	{

		for (i = 0; i < 3; i++)
		{
				
			// wait until either socket has data ready to be recv()d (timeout 10.5 secs)
		        timeout.tv_sec = 10;
        		timeout.tv_usec = 500000;

			// select function
			n_sel = select(nfds, &readfds_c, NULL, NULL, &timeout);

			if(n_sel > 0)
				recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &myaddr, &len);
		}
	}

	else
	{
		printf("Nobody hears you");
		exit(snd);
	}
	


	exit(0);


}
