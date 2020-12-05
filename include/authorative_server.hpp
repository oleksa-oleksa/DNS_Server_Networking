//
// Created by Oleksandra Baga on 29.11.20.
//

#ifndef TELEMATIC_PROJECTS_2021_AUTHORATIVE_SERVER_H
#define TELEMATIC_PROJECTS_2021_AUTHORATIVE_SERVER_H

#include <string>
using namespace std;

struct DnsRequest {
    int dns_flags_response;
    int dns_flags_recdesired;
    string dns_qry_name;
    int dns_qry_type;
    string dns_a;
    int dns_flags_authoritative;
};

typedef struct DnsRequest DnsRequest;

#endif //TELEMATIC_PROJECTS_2021_AUTHORATIVE_SERVER_H
