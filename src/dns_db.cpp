//
// Created by Oleksandra Baga on 29.11.20.
//

#include "dns_db.h"
#include <algorithm>
#include <memory>

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