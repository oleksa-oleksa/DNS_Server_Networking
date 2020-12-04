#include <stdio.h>
#include <string>
#include <json-c/json.h>
#include <unistd.h>
#include <iostream>

#include "../include/udp.hpp"
#include "../include/dns_db.hpp"
#include "../include/authorative_server.hpp"

#define RECURSIVE_RESOLVER_IP 	"127.0.0.2"
#define ROOT_SERVER_IP          "127.0.0.10"
#define MAXCHAR 100

using namespace std;
using namespace udp;

bool parse_from_json(const char *json, DnsRequest *request)
{
    // Creating a json object *//*
    json_object *parsed_json = json_tokener_parse(json);

    json_object *tmp;
    if (json_object_object_get_ex(parsed_json, "dns.flags.response", &tmp)) {
        request->dns_flags_response = json_object_get_int(tmp);
    }
    if (json_object_object_get_ex(parsed_json, "dns.flags.recdesired", &tmp)) {
        request->dns_flags_recdesired = json_object_get_int(tmp);
    }

    if (json_object_object_get_ex(parsed_json, "dns.qry.name", &tmp)) {
        request->dns_qry_name = json_object_get_string(tmp);
    }

    if (json_object_object_get_ex(parsed_json, "dns.qry.type", &tmp)) {
        request->dns_qry_type = json_object_get_int(tmp);
    }

    if (json_object_object_get_ex(parsed_json, "dns.a", &tmp)) {
        request->dns_a = json_object_get_string(tmp);
    }

    if (json_object_object_get_ex(parsed_json, "dns.flags.authoritative", &tmp)) {
        request->dns_flags_authoritative = json_object_get_int(tmp);
    }

    return 0;
}

void parse_request(const char *name, char *temp_buf) {
    /* Creating a json object */
    json_object *jobj = json_object_new_object();

    json_object *dns_flags_response = json_object_new_int(0);
    json_object *dns_flags_recdesired = json_object_new_int(1);
    json_object *dns_qry_name = json_object_new_string(name);
    json_object *dns_qry_type = json_object_new_int(1);

    /* Form the json object as set of key-value pairs */
    json_object_object_add(jobj, "dns.flags.response", dns_flags_response);
    json_object_object_add(jobj, "dns.flags.recdesired", dns_flags_recdesired);
    json_object_object_add(jobj, "dns.qry.name", dns_qry_name);
    json_object_object_add(jobj, "dns.qry.type", dns_qry_type);

    if (strcpy(temp_buf, json_object_to_json_string(jobj)) == 0)
        perror("strcpy");
}

int main() {
    printf("Recursive resolver started.\n");

    DnsRequest request;

    Udp udp(RECURSIVE_RESOLVER_IP, UDPPORT);
    string recvmsg; // received message
    string remaddr; // remote address
    string stubaddr; //stub address

    while (1)
    {
        udp.recv(recvmsg, remaddr);
        puts("RECV");
        printf("Server: Received message \"%s\" from %s\n", recvmsg.c_str(), remaddr.c_str());
        parse_from_json(recvmsg.c_str(), &request);

        if (request.dns_flags_recdesired == 1) {
            // Request from stub
            stubaddr = remaddr;
            udp.send(recvmsg, ROOT_SERVER_IP);
            puts("SEND");
        } else if(request.dns_flags_authoritative) {
            // Autho answer
            std::cout << stubaddr << "\n";
            udp.send(recvmsg, stubaddr);
            puts("SEND");
        } else {
            // Answer from server
            char temp_buf[BUFSIZE];
            parse_request(request.dns_qry_name.c_str(), temp_buf);
            string sndstr = string(temp_buf);
            udp.send(sndstr, request.dns_a);
            puts("SEND");
        }     
    }

    return 0;
}
