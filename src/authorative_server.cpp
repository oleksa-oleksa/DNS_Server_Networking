#include <stdio.h>
#include <string>
#include <json-c/json.h>
#include <unistd.h>

#include "udp.hpp"

#define SERVER_IP 	"127.0.0.10"

using namespace std;
using namespace udp;

int main()
{
	printf("Authorative server startet.\n");
	string hello = "ACK";

    Udp udp(SERVER_IP, UDPPORT);
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

