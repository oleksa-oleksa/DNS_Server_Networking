#include <stdio.h>
#include <string>
#include <json-c/json.h>
#include <unistd.h>

#include "udp.hpp"

#define SERVER_IP 	"127.0.0.10"

using namespace std;
using namespace udp;

int main(int argc, char **argv)
{
    char *server_ip;
    char *server_name;

    if (argc != 3) {
        printf("command line parameter: server_ip name");
        return -2;
    }
    else {
        server_ip = argv[1];
        server_name = argv[2];
    }

    printf("Server started: %s: %s\n", server_ip, server_name);
	string hello = "ACK";

    Udp udp(server_ip, UDPPORT);
    string recvmsg;   // received message
    string remaddr;   // remote address

	while(1)
   {
        udp.recv(recvmsg, remaddr);
        printf("Server: Received message \"%s\" from %s\n", recvmsg.c_str(), remaddr.c_str());

		printf("Client : %s\n", recvmsg.c_str());
        udp.send(hello, remaddr);
		printf("Sent ACK.\n");
	}

	return 0;
}

