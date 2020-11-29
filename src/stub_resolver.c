#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <json-c/json.h>

#define PORT 		53053
#define MAXLINE 	1024
#define SERVER_IP 	"127.0.0.10"
#define RESOLVER_IP	"127.0.0.1"

void parse_to_json(char *input, char *temp_buf) {
	/*Creating a json object*/
	json_object *jobj = json_object_new_object();

	json_object *dns_flags_response = json_object_new_int(0);
	json_object *dns_flags_recdesired = json_object_new_int(1);
	json_object *dns_qry_name = json_object_new_string(input);
	json_object *dns_qry_type = json_object_new_int(1);

	/*Form the json object*/
	/*Each of these is like a key value pair*/
	json_object_object_add(jobj, "dns.flags.response", dns_flags_response);
	json_object_object_add(jobj, "dns.flags.recdesired", dns_flags_recdesired);
	json_object_object_add(jobj, "dns.qry.name", dns_qry_name);
	json_object_object_add(jobj, "dns.qry.type", dns_qry_type);

	printf("Size of JSON_TO_STRING- %lu,\n %s\n", sizeof(json_object_to_json_string(jobj)), json_object_to_json_string(jobj));

	if (strcpy(temp_buf, json_object_to_json_string(jobj)) == NULL)
	{
		perror("strcpy");
		// return EXIT_FAILURE;
	}
}

int main() {
	printf("Resolver startet.\n");
	int sockfd;
	char recv_buf[MAXLINE];
	char *hello = "Hello from client";
	struct sockaddr_in	 serv_addr;
	char input[64];

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	
	// Filling server information
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int n, len;

	while(1){
		char temp_buf[MAXLINE];

		scanf("%s", input);
		parse_to_json(input, temp_buf);

		sendto(sockfd, temp_buf, strlen(temp_buf),
			   MSG_CONFIRM, (const struct sockaddr *)&serv_addr,
			   sizeof(serv_addr));
		printf("Sent message to server.\n");
		
		n = recvfrom(sockfd, (char *)recv_buf, MAXLINE,
					 MSG_WAITALL, (struct sockaddr *) &serv_addr,
					 &len);
		recv_buf[n] = '\0';
		printf("Server: %s\n", recv_buf);
	}

	close(sockfd);
	return 0;
}
