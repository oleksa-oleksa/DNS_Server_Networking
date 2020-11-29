//
// Created by Oleksandra Baga on 29.11.20.
//

#include "dns_db.h"

bool DnsDb::add_record(DnsRecord *record) {
    new_record = &record
    db.push_back(&record);
}