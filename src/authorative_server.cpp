#include <stdio.h>
#include <string>
#include <json-c/json.h>
#include <unistd.h>

#include "udp.hpp"
#include "dns_db.h"
#include "authorative_server.h"

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
        //printf("%s", str);
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



bool parse_from_json(const char *json, DnsRequest *request)
{
 // Creating a json object *//*
    json_object* parsed_json = json_tokener_parse(json);

    json_object *tmp;
    if (json_object_object_get_ex(parsed_json, "dns.flags.response", &tmp)) {
        // Key exists
        request->dns_flags_response = json_object_get_int(tmp);
    }
    if (json_object_object_get_ex(parsed_json, "dns.flags.recdesired", &tmp)) {
        // Key exists
        request->dns_flags_recdesired = json_object_get_int(tmp);
    }

    if (json_object_object_get_ex(parsed_json, "dns.qry.name", &tmp)) {
        // Key exists
        request->dns_qry_name = json_object_get_string(tmp);
    }

    if (json_object_object_get_ex(parsed_json, "dns.qry.type", &tmp)) {
        // Key exists
        request->dns_qry_type = json_object_get_int(tmp);
    }

    return 0;

}


int main(int argc, char **argv)
{
    char *server_ip;
    char *server_name;
    DnsDb db;
    DnsRequest request;

    if (argc != 3) {
        printf("command line parameter: server_ip name");
        return -2;
    }
    else {
        server_ip = argv[1];
        server_name = argv[2];
    }

    printf("Server started: %s: %s\n", server_ip, server_name);

	read_config(server_name, &db);
    // debug
	// printf("%s\n", db.find_record("telematik")->ip.c_str());

    Udp udp(server_ip, UDPPORT);
    string recvmsg;   // received message
    string remaddr;   // remote address


    while(1)
   {
        udp.recv(recvmsg, remaddr);
        printf("Server: Received message \"%s\" from %s\n", recvmsg.c_str(), remaddr.c_str());
        parse_from_json(recvmsg.c_str(), &request);

       const DnsRecord *found = db.find_record(request.dns_qry_name);
       if (found != NULL) {
           if(found->ip.c_str() != NULL) {
               printf("Client request: %s\n", recvmsg.c_str());
               udp.send("FOUND!", remaddr);
           }
           else {
               printf("Client request: %s\n", recvmsg.c_str());
               udp.send("NO IP: REFUSED", remaddr);
           }
       }
       else {
           printf("Client request: %s\n", recvmsg.c_str());
           udp.send("NO DNS RECORD: REFUSED!", remaddr);
       }

		// printf("Client : %s\n", recvmsg.c_str());
        //udp.send(hello, remaddr);
		//printf("Sent ACK.\n");
	}

	return 0;
}

