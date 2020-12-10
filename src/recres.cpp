/*
* recres.cpp
** Recursive Resolver
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
#define ROOT "ns."

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
    string remaddr; // remote address
    string msg, query, response; // DNS message, query resp. response Json string
    string rootNsIp = db.findIp(ROOT);
    string stubIP;
    /*
    * DNS - Authoritative Name Server algorithm
    */
    while(1)
    {
        // receive a DNS packet
        udp.recv(msg, remaddr);
        dns.fromJson(msg);

        // a query was received (from Stub)
        if (dns.flags_recdesired)
        {
            stubIP = remaddr;
            udp.send(msg, rootNsIp);
        }
        // No result found
        else if (dns.flags_rcode == 10)
        {
            udp.send(msg, stubIP);
        }
        // a response was received
         else if(dns.flags_response && !dns.flags_authoritative)
        {
            string ip = dns.a;
            query = dns.query(dns.qry_name.substr(0, dns.qry_name.size() - 1), "A");

            udp.send(query, ip);
            /*
            * TODO: Evaluate whether the response carries a reference
            * to another NS or the authoritative name resolution.
            */
        }
        // a response was received (from Auth.NS)
        else if (dns.flags_response && dns.flags_authoritative)
        {
            udp.send(msg, stubIP);
        }
    }

    return 0;
}
