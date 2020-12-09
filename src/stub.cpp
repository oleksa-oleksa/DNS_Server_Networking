/*
* stub.cpp
*
* Stub Resolver
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

// name of the pre-configured local name server (recursive resolver)
#define RECRESOLVER "dns.server.local"

using namespace std;
//using namespace udp;

/*
*/
int main(int argc, char** argv)
{
    /*
    * Initializations
    */

    if(argc != 2)
    {
        cout << "Usage: ./stub [name]" << endl;
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
        cout << "Error: No address entry for for this host!" << endl;
        return 1;
    }

    // look up address of pre-configured name server
    string localNsIp = db.findIp(RECRESOLVER);
    // string localNsIp = "127.0.0.21"; // for development only, for production see a line above !
    // string localNsIp = "127.0.0.1"; // root NS, for development only, for production see a line above !
    if(localNsIp == "")
    {
        cout << "Error: No address entry for pre-configured DNS name server!" << endl;
        return 2;
    }

    // start UDP server
    Udp udp(ip, UDPPORT);
    cout << "DNS Stub Resolver " << name << " started on " << ip << endl;

    DnsPacket dns;
    DnsRecord rec;
    string remaddr; // remote address
    string query, response;   // DNS query Json string

    /*
    * DNS - Stub Resolver algorithm
    */
    while(1)
    {
        cout << "Enter a host's name to query its IP: ";
        cin >> name;

        // try to answer from cache first
        ip = db.findIp(name + ".");

        // no corresponding record in cache, so DNS has to be queried
        if(ip == "")
        {
            // send query to the pre-configured local NS (recursive resolver)
            query = dns.query(name, "A");
            udp.send(query, localNsIp);
            // cout << "STUB sent a query to " << localNsIp << ":\n" << query << endl;
            cout << "STUB sent a query to " << localNsIp << "\n";

            // receive the response
            /*
            * TODO: Check whether this is indeed an answer to the query
            */
            udp.recv(response, remaddr);
            // cout << "STUB received a response from " << remaddr << ":\n" << response;
            dns.fromJson(response);

            // write the info into a new or refreshed record in database
            rec.label = dns.resp_name;
            rec.ttl = dns.resp_ttl;
            rec.type = dns.resp_type;
            rec.value = dns.a;
            db.write(rec);

            ip = dns.a; // only needed for message print
        }
        // a corresponding record was found in the cache
        else
            cout << "A record was found in the cache.\n";

        if(dns.flags_rcode == 10)
            cout << "Error: This host couldn't be found!" << endl<<endl;
        else
            cout << "Host " << dns.resp_name << " has the IP address " << ip << ".\n\n";
    }

    return 0;
}
