#include <stdio.h>
#include <string>
#include <json-c/json.h>
#include <unistd.h>

#include "udp.hpp"
#include "dns_db.h"

//#define SERVER_IP 	"127.0.0.10"
#define MAXCHAR 100

using namespace std;
using namespace udp;

bool read_config(const char *server_name, DnsDb *db) {
    FILE *fp;
    char str[MAXCHAR];
    string filename = string("dns_db/") + server_name;

    fp = fopen(filename.c_str(), "r");
    if (fp == NULL){
        printf("Could not open file %s", filename.c_str());
        return 1;
    }
    char s1[20], s2[20];
    while (fgets(str, MAXCHAR, fp) != NULL) {
        printf("%s", str);
        sscanf(str, "%s %s", s1, s2);
        //printf("GOT: %s: %s\n", s1, s2);
        DnsRecord record;
        record.ip = s1;
        record.domain = s2;
        record.record_type = 1;
        db->add_record(record);
    }
    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
    char *server_ip;
    char *server_name;
    DnsDb db;

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

	read_config(server_name, &db);
	printf("%s", db.find_record("telematik")->ip.c_str());

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

