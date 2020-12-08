/*
* authns.cpp
*
* Authoritative Name Server
*
* Oleksandra Baga
* Milos Budimir
* Jannes Volkens
* Michael Zent
*
* Telematik, Prof. M. Waehlisch
* WS 2020/21 FU Berlin
*/

#include "dns_packet.h"
#include "dns_db.h"
#include "udp.h"

using namespace std;

/*
*/
int main(int argc, char** argv)
{
    /*
    * Initializations
    */

    if(argc != 2)
    {
        cout << "Usage: ./auth_ns [name]" << endl<<endl;
        return 1;
    }

    string name(argv[1]); // read own host name from command line

    DnsDb db;
    db.fromFile("dns_db/config/" + name); // read out corresponding config file
    name += "."; // add dot to create fully qualified domain name (FQDN)

    // look up own IP address
    string ip = db.findIp(name);
    if(ip == "")
    {
        cout << "Error: Configuration lacks an record for this host!" << endl<<endl;
        return 1;
    }

    // start UDP server
    Udp udp(ip, UDPPORT);
    cout << "Authoritative NS " << name << " started on " << ip << endl<<endl;

    DnsPacket dns;
    DnsRecord* rec;
    string remaddr; // remote address
    string domain; // domain name
    string query, response;   // DNS query resp. response Json string

    /*
    * DNS - Authoritative Name Server algorithm
    */
    while(1)
    {
        // receive a DNS packet
        udp.recv(query, remaddr);
        dns.fromJson(query);

        // a query was received indeed
        if(!dns.flags_response)
        {
            cout << "AUTH.NS received query from " << remaddr << ":\n" << query << endl;

            // try to answer from cache first
            rec = db.find(dns.qry_name);

            if(rec == NULL) // no record
            {
                /*
                * Read the domain suffixes and try to find a corresponding NS in the database
                */
                uint16_t j = 0;
                for(uint16_t i = dns.qry_name.size()-2; i > 0 && i < dns.qry_name.size(); i = j-1)
                {
                    if((j = dns.qry_name.rfind('.', i)) == (uint16_t)string::npos){
                        ip = "";
                        break;
                    }

                    domain = dns.qry_name.substr(j+1);
                    rec = db.find(domain);
                    if(rec && rec->type == "NS")
                    {
                        // get IP address of this NS
                        rec = db.find(rec->value);
                        if(rec)
                        {
                            ip = rec->value; // NS address
                            break;
                        }
                    }
                }

                if(ip == "") // send response with error code
                {
                    dns.flags_response = 1;
                    dns.flags_rcode = 10;
                    dns.flags_recavail = 0;
                    dns.flags_recdesired = 0;
                    dns.count_answers = 0;
                    dns.flags_authoritative = 0;
                }
                else // send response with reference to another NS
                {
                    dns.flags_response = 1;
                    dns.flags_recavail = 0;
                    dns.flags_recdesired = 0;
                    dns.flags_rcode = 0;
                    dns.count_answers = 0;
                    dns.flags_authoritative = 0;
                    // the NS record is part of the authoritative section
                    dns.count_auth_rr = 1;
                    dns.resp_name = domain; // domain name
                    dns.resp_ttl_ns = 86400;
                    dns.resp_type = 2; // NS
                    dns.ns = rec->label; // NS name
                    // the corresponding A record is part of the additional section
                    dns.count_add_rr = 1;
                    dns.resp_ttl_a = 300;
                    dns.a = ip;
                }
            }
            else // a corresponding record was found in cache
            {
                // create a response packet, the fields 'qry_name' and 'qry_type' are unchanged
                dns.flags_response = 1;
                dns.flags_recavail = 0;
                dns.flags_recdesired = 0;
                dns.flags_rcode = 0;
                dns.count_answers = 1;
                dns.flags_authoritative = 1;
                dns.resp_name = dns.qry_name;
                dns.resp_type = 1;
                dns.resp_ttl = 300;

                if(rec && rec->type == "NS") {
                    // get IP address of this NS
                    rec = db.find(rec->value);
                    if(rec) {
                        dns.a = rec->value; // NS address
                    }
                } else {
                    dns.a = rec->value;
                }
            }

            response = dns.toJson();
            udp.send(response, remaddr);
            dns.clear(); // clear dns packet before next query
            cout << "AUTH.NS sent response to " << remaddr << ":\n" << response << endl;
        }
    }

    return 0;
}
