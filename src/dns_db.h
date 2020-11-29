//
// Created by Oleksandra Baga on 29.11.20.
//

#ifndef TELEMATIC_PROJECTS_2021_DNS_DB_H
#define TELEMATIC_PROJECTS_2021_DNS_DB_H
#include <string>
#include <vector>

using namespace std;

struct DnsRecord {
    string ip;
    strind domain;
    // Flags see https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml#dns-parameters-4
    unsigned short record_type;
};
typedef struct DnsRecord DnsRecord;

class DnsDb{
private:
    vector<DnsRecord*> db;
    //list<DnsRecord>
    //hash<>
    //set>>
public:
    DbsDb() : db() {};

    DnsRecord *find_record(string name);
    bool add_record(DnsRecord *record);
};


#endif //TELEMATIC_PROJECTS_2021_DNS_DB_H
