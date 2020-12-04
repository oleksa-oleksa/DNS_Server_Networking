#include <stdio.h>
#include <string>
#include <json-c/json.h>
#include <unistd.h>
#include <iostream>

#include "../include/udp.hpp"
#include "../include/dns_db.hpp"
#include "../include/authorative_server.hpp"

//#define SERVER_IP 	"127.0.0.10"
#define MAXCHAR 100
#define AUTHORITATIVE       1
#define NON_AUTHORITATIVE   0

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



bool parse_from_json(const char *json, DnsRequest *request) {
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

void parse_msg(char *temp_buf, const char *name, const char *ip, const char *domain, int authoritative) {
    /* Creating a json object */
    json_object *jobj = json_object_new_object();

    json_object *dns_flags_response = json_object_new_int(1);
    json_object *dns_flags_recdesired = json_object_new_int(0);
    json_object *dns_qry_name = json_object_new_string(name);
    json_object *dns_qry_type = json_object_new_int(1);
    json_object *dns_a = json_object_new_string(ip);
    json_object *dns_ns = json_object_new_string(domain);
    json_object *dns_flags_authoritative = json_object_new_int(authoritative);

    /* Form the json object as set of key-value pairs */
    json_object_object_add(jobj, "dns.flags.response", dns_flags_response);
    json_object_object_add(jobj, "dns.flags.recdesired", dns_flags_recdesired);
    json_object_object_add(jobj, "dns.qry.name", dns_qry_name);
    json_object_object_add(jobj, "dns.qry.type", dns_qry_type);
    json_object_object_add(jobj, "dns.a", dns_a);
    json_object_object_add(jobj, "dns.ns", dns_ns);
    json_object_object_add(jobj, "dns.flags.authoritative", dns_flags_authoritative);

    if (strcpy(temp_buf, json_object_to_json_string(jobj)) == 0)
        perror("strcpy");
}

int main(int argc, char **argv) {
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

    Udp udp(server_ip, UDPPORT);
    string recvmsg;   // received message
    string remaddr;   // remote address

    while(1) {
        udp.recv(recvmsg, remaddr);
        puts("RECV");
        char temp_buf[BUFSIZE];
        parse_from_json(recvmsg.c_str(), &request);

        const DnsRecord *found = db.find_record(db.get_relevant_record(request.dns_qry_name, server_name));
        printf("NAME: %s IP: %s\n", found->domain.c_str(), found->ip.c_str());

        if (found != NULL) {
            if(found->ip.c_str() != NULL && found->domain.c_str() != request.dns_qry_name) {
                parse_msg(temp_buf, request.dns_qry_name.c_str(), found->ip.c_str(), found->domain.c_str(), NON_AUTHORITATIVE);
                string sndstr = string(temp_buf);
                udp.send(sndstr, remaddr);
                puts("SEND");
            }
            // AUTHORITATIVE
            else if(found->ip.c_str() != NULL && found->domain.c_str() == request.dns_qry_name) {
                parse_msg(temp_buf, request.dns_qry_name.c_str(), found->ip.c_str(), found->domain.c_str(), AUTHORITATIVE);
                string sndstr = string(temp_buf);
                udp.send(sndstr, remaddr);
                puts("SEND");
            } else {
                printf("Client request: %s\n", recvmsg.c_str());
                udp.send("NO IP: REFUSED", remaddr);
                puts("SEND");
            }
        }
        else {
            printf("Client request: %s\n", recvmsg.c_str());
            udp.send("NO DNS RECORD: REFUSED!", remaddr);
            puts("SEND");
        }
	}

	return 0;
}

