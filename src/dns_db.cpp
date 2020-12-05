//
// Created by Oleksandra Baga on 29.11.20.
//

#include "../include/dns_db.hpp"
#include <algorithm>
#include <memory>
#include <iostream>

bool DnsDb::add_record(const DnsRecord &new_record) {

    const DnsRecord *existing = find_record(new_record.domain);
    if (existing == NULL) {
        DnsRecord *p = new DnsRecord(new_record);
        db.push_back(shared_ptr<DnsRecord>(p));
        return true;
    } else {
        return false;
    }
}

const DnsRecord * DnsDb::find_record(const string &domain) {

    for (auto it = db.begin(); it != db.end(); it++) {
        const DnsRecord *r = it->get();
        if (r->domain == domain) {
            return r;
        }
    }
    return NULL;
}

const string DnsDb::get_relevant_record(const string domain, const string server_name) {

    int pos = 0;

    // For root server
    if (server_name == "root") {
        for (int i = domain.length() - 1; i >= 0; i--) {
            if (domain[i] == '.'){
                pos = i;
                break;
            }    
        }

        if (pos == 0) {
            return domain;
        }

        return domain.substr(pos + 1);
    }

    for (int i = (domain.length() - server_name.length()) - 1; i >= 0; i--) {
        if (domain[i] == '.' && i != int(domain.length() - server_name.length() - 1)) {
            pos = i;
            break;
        }
    }

    if (pos == 0) {
        return domain;
    }

    return domain.substr(pos + 1);
}