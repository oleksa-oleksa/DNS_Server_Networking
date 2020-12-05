//
// Created by Oleksandra Baga on 29.11.20.
//

#ifndef TELEMATIC_PROJECTS_2021_DNS_DB_H
#define TELEMATIC_PROJECTS_2021_DNS_DB_H
#include <string>
#include <vector>
#include <memory>

using namespace std;

struct DnsRecord {
    string ip;
    string domain;
    // Flags see https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml#dns-parameters-4
    unsigned short record_type;
};

typedef struct DnsRecord DnsRecord;

class DnsDb {
private:
    typedef shared_ptr<DnsRecord> PDnsRecord;
    vector<PDnsRecord> db;

public:
    DnsDb() : db() {};
    DnsDb(const DnsDb &other) : db(other.db) {}

    const DnsRecord *find_record(const string &name);
    const string get_relevant_record(const string domain, const string server_name);
    bool add_record(const DnsRecord &record);
};


#endif //TELEMATIC_PROJECTS_2021_DNS_DB_H
