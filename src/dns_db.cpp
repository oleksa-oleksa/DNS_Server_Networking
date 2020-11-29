//
// Created by Oleksandra Baga on 29.11.20.
//

#include "dns_db.h"
#include <algorithm>

bool DnsDb::add_record(DnsRecord *new_record) {

    const DnsRecord *existing = find_record(new_record->domain);
    if (existing == NULL) {
        db.push_back(shared_ptr<DnsRecord>(new_record));
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