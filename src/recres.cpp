/*
* recres.cpp
*
* Recursive Resolver
*
* Oleksandra Baga
* Milos Budimir
* Jannes Volkens
* Michael Zent
*
* Telematik, Prof. M. Wählisch
* WS 2020/21 FU Berlin
*/

#include "dns_packet.h"
#include "dns_db.h"
#include "udp.h"

// name of the pre-configured local name server (recursive resolver)
#define RECRESOLVER "dns.server.local"

using namespace std;

/*
*/
int main(void)
{
    /*
    * Initializations
    */

    string name = RECRESOLVER;

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
    cout << "Recursive Resolver " << name << " started on " << ip << endl<<endl;

    DnsPacket dns;
    DnsRecord* rec;
    string remaddr; // remote address
    string msg, query, response; // DNS message, query resp. response Json string

    /*
    * DNS - Authoritative Name Server algorithm
    */
    while(1)
    {
        // receive a DNS packet
        udp.recv(msg, remaddr);
        dns.fromJson(msg);

        // a response was received (from Auth.NS)
        if(dns.flags_response)
        {
            /*
            * TODO: Evaluate whether the response carries a reference
            * to another NS or the authoritative name resolution.
            */
        }
        // a query was received (from Stub)
        else
        {
            /*
            * TODO: Start iterative querying of authoritative NSs
            */
        }
    }

    return 0;
}
