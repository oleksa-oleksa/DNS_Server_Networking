#include <stdio.h>
#include <string>
#include <json-c/json.h>
#include <unistd.h>

#include "../include/udp.hpp"

#define RECURSIVE_RESOLVER_IP   "127.0.0.2"
#define STUB_RESOLVER_IP        "127.0.0.1"

using namespace std;
using namespace udp;

void parse_to_json(char* input, char* temp_buf) {
	/* Creating a json object */
	json_object* jobj = json_object_new_object();

	json_object* dns_flags_response = json_object_new_int(0);
	json_object* dns_flags_recdesired = json_object_new_int(1);
	json_object* dns_qry_name = json_object_new_string(input);
	json_object* dns_qry_type = json_object_new_int(1);

	/* Form the json object as set of key-value pairs */
	json_object_object_add(jobj, "dns.flags.response", dns_flags_response);
	json_object_object_add(jobj, "dns.flags.recdesired", dns_flags_recdesired);
	json_object_object_add(jobj, "dns.qry.name", dns_qry_name);
	json_object_object_add(jobj, "dns.qry.type", dns_qry_type);

	if(strcpy(temp_buf, json_object_to_json_string(jobj)) == 0)
		perror("strcpy");
}

int main() {
	printf("Stub Resolver starts.\n");
	string hello = "Hello from client";
	char input[64];

	Udp udp(STUB_RESOLVER_IP, UDPPORT);
	string recvmsg;   // received message
    	string remaddr;   // remote address

	while(1) {
		
		char temp_buf[BUFSIZE];

		scanf("%s", input);
		parse_to_json(input, temp_buf);

        	string sndstr = string(temp_buf);
		udp.send(sndstr, RECURSIVE_RESOLVER_IP);
		// printf("Sent message \"%s\" to server at %s:%d.\n", sndstr.c_str(), SERVER_IP, UDPPORT);
		puts("SEND");

		udp.recv(recvmsg, remaddr);
		printf("Server: Received message \"%s\" from %s\n", recvmsg.c_str(), remaddr.c_str());
	}

	return 0;
}
