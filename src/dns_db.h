/*
* dns_db.h
*
* DNS Database Interface
*
* Oleksandra Baga
* Milos Budimir
* Jannes Volkens
* Michael Zent
*
* Telematik, Prof. M. W�hlisch
* WS 2020/21 FU Berlin
*/

#ifndef _DNS_DB_H
#define _DNS_DB_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <ctime>

using namespace std;

struct DnsRecord
{
    string label;
    uint32_t ttl;
    string type;
    string value;
};

typedef struct DnsRecord DnsRecord;

/*
*/
class DnsDb
{
private:
    typedef shared_ptr<DnsRecord> PDnsRecord;
    vector<PDnsRecord> db;
    time_t timestamp;

public:
    DnsDb() : db() {timestamp = time(NULL);};
    DnsDb(const DnsDb &other) : db(other.db) {}

    /*
    * Find database record according to given label
    */
    DnsRecord* find(const string& label)
    {
        /*
        * Update TTLs, remove outdated records
        */
        time_t now = time(NULL);
        time_t diff = now - this->timestamp;
        this->timestamp = now;
        for(auto it = db.begin(); it != db.end(); it++)
        {
            DnsRecord* r = it->get();
            if(r->ttl <= diff)
                db.erase(it--);
            else
                r->ttl -= diff;
        }

        for(auto it = db.begin(); it != db.end(); ++it)
        {
            DnsRecord* r = it->get();
            if(r->label == label)
                return r;
        }

        return NULL;
    }

    /*
    * Return the IP address of the given host
    */
    string findIp(const string& host)
    {
        DnsRecord* rec = find(host);
        if(rec)
            return rec->value;

        return "";
    }

    /*
    * Add given DNS record to database. Don't overwrite if already existing.
    */
    bool add(const DnsRecord& rec)
    {
        DnsRecord* existing = find(rec.label);

        if(existing)
            return false;

        DnsRecord* r = new DnsRecord(rec);
        db.push_back(shared_ptr<DnsRecord>(r));
        return true;
    }

    /*
    * Write given DNS record to database. Overwrite if already existing.
    */
    void write(const DnsRecord& rec)
    {
        DnsRecord* existing = find(rec.label);

        if(existing)
        {
            existing->ttl = rec.ttl;
            existing->type = rec.type;
            existing->value = rec.value;
            return;
        }

        DnsRecord* r = new DnsRecord(rec);
        db.push_back(shared_ptr<DnsRecord>(r));
    }

    /*
    * Write database records having read from file
    */
    bool fromFile(string path)
    {
        FILE* fp;
        char str[512];
        char label[128], ttl[16], type[8], value[128];

        fp = fopen(path.c_str(), "r");
        if(fp == NULL)
        {
            cout << "Could not open file " << path << endl;
            return 1;
        }

        while(fgets(str, 512, fp) != NULL)
        {
            sscanf(str, "%s %s %s %s", label, ttl, type, value);
            DnsRecord rec;
            rec.label = string(label);
            rec.ttl = stoi(string(ttl));
            rec.type = string(type);
            rec.value = string(value);
            write(rec);
        }

        fclose(fp);
        return 0;
    }


    /*
    * Output db to a file.
    */
    void toFile(string path)
    {
        ofstream outputFile;
        outputFile.open(path);
        for(auto it = db.begin(); it != db.end(); ++it)
        {
            DnsRecord* r = it->get();
            outputFile << r->label << " ";
            outputFile << r->ttl << " ";
            outputFile << r->type << " ";
            outputFile << r->value << endl;
        }

        outputFile.close();
    }
};

#endif // _DNS_DB_H
