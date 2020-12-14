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

#include <fstream> // for outputing log files
#include <iostream>
#include <ctime> // for delay
#include <chrono>
#include <thread>
#include <cstdlib> //for random number

#include "dns_packet.h"
#include "dns_db.h"
#include "udp.h"

// name of the pre-configured local name server (recursive resolver)
#define RECRESOLVER "dns.server.local"
#define ROOT "ns."

using namespace std;
using namespace std::this_thread; // sleep_for
using namespace std::chrono; // milliseconds

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
    DnsRecord rec;
    string remaddr; // remote address
    string msg, query, response; // DNS message, query resp. response Json string
    string rootNsIp = db.findIp(ROOT);
    string stubIP;

    // create log and debug file for stub
    ofstream logFile, debugFile;
    logFile.open("./log/recres/" + name + "log", ios::out);
    debugFile.open("./debug/recres/" + name + "txt", ios::out);

    time_t now;

    // vars used in log outputs
    int num_of_requests_sent = 0;
    int num_of_requests_received = 0;
    int num_of_responses_sent = 0;
    int num_of_responses_received = 0;

    /*
    * DNS - Authoritative Name Server algorithm
    */
    while(1)
    {
        // receive a DNS packet
        udp.recv(msg, remaddr);
        dns.fromJson(msg);

        if(dns.flags_response) // answer from Auth.NS
        {
            ++num_of_responses_received;
            now = time(0); // get current time
            logFile << now << " | " << remaddr << " | " << num_of_requests_sent << " | " << num_of_requests_received << " | " << num_of_responses_sent << " | " << num_of_responses_received << endl;
            debugFile << "Received from " << remaddr << ": " << msg << endl;
cout << "Received from " << remaddr << ": " << msg << endl;

            // No result found
            if(dns.flags_rcode == 10)
            {
                sleep_for(milliseconds(rand() % 100 + 100)); //delay random 100ms to 200ms
                udp.send(msg, stubIP);

                ++num_of_responses_sent;
                now = time(0); // get current time
                logFile << now << " | " << stubIP << " | " << num_of_requests_sent << " | " << num_of_requests_received << " | " << num_of_responses_sent << " | " << num_of_responses_received << endl;
                debugFile << "Sent to " << stubIP << ": " << msg << endl;
cout << "Sent to " << stubIP << ": " << msg << endl;
            }
            // an authoritative answer was received (from Auth.NS)
            else if(dns.flags_authoritative && dns.count_answers)
            {
                // write the info into a new or refreshed record in database
                rec.label = dns.resp_name;
                rec.ttl = dns.resp_ttl;
                rec.type = dns.resp_type;
                rec.value = dns.a;
                db.write(rec);

                sleep_for(milliseconds(rand() % 100 + 100)); //delay random 100ms to 200ms
                udp.send(msg, stubIP);

                ++num_of_responses_sent;
                now = time(0); // get current time
                logFile << now << " | " << stubIP << " | " << num_of_requests_sent << " | " << num_of_requests_received << " | " << num_of_responses_sent << " | " << num_of_responses_received << endl;
                debugFile << "Sent to " << stubIP << ": " << msg << endl;
cout << "Sent to " << stubIP << ": " << msg << endl;
            }
            // reference to another NS was received (from Auth.NS)
            else if(dns.count_auth_rr && dns.resp_type == 2)
            {
cout << "AUTHNSREF" << endl;

                // cache NS entry
                rec.label = dns.resp_name; // domain
                rec.ttl = dns.resp_ttl_ns;
                rec.type = dns.resp_type;
                rec.value = dns.ns;
                db.write(rec);
cout << "NEWNSREC" << endl;

                // cache corresponding A entry
                rec.label = dns.ns;
                rec.ttl = dns.resp_ttl_a;
                rec.type = 1;
                rec.value = dns.a;
                db.write(rec);
cout << "NEWAREC" << endl;

                // query that NS
                ip = dns.a;
                msg = dns.query(dns.qry_name.substr(0, dns.qry_name.size() - 1), "A");
                sleep_for(milliseconds(rand() % 100 + 100)); //delay random 100ms to 200ms
                udp.send(msg, ip);

                // log output and increment num
                num_of_requests_sent++;
                time_t now = time(0); // get current time
                logFile << now << " | " << ip << " | " << num_of_requests_sent  << " | " << "0" << " | " << "0"  << " | " << num_of_responses_received << endl;
                debugFile << "Sent to " << ip << ": " << query << endl;
cout << "Sent to " << ip << ": " << query << endl;
            }
        }
        else // query from stub resolver
        {
            stubIP = remaddr;

            ++num_of_requests_received;
            now = time(0); // get current time
            logFile << now << " | " << remaddr << " | " << num_of_requests_sent << " | " << num_of_requests_received << " | " << num_of_responses_sent << " | " << num_of_responses_received << endl;
            debugFile << "Received from " << remaddr << ": " << msg << endl;
cout << "Received from " << remaddr << ": " << msg << endl;

            // try to answer from cache first
            ip = db.findIp(dns.qry_name + ".");

            // no corresponding record in cache, so DNS has to be queried
            if(ip == "" && dns.flags_recdesired)
            {
                /*
                * Read the domain suffixes and try to find a corresponding NS in the database
                */
                uint16_t j = 0;
                for(uint16_t i = dns.qry_name.size()-2; i > 0 && i < dns.qry_name.size(); i = j-1)
                {
                    if((j = dns.qry_name.rfind('.', i)) == (uint16_t)string::npos)
                    {
                        ip = "";
                        break;
                    }

                    string domain = dns.qry_name.substr(j+1);
                    DnsRecord* r = db.find(domain);
                    if(r && r->type == "NS")
                    {
                        // get IP address of this NS
                        r = db.find(rec.value);
                        if(r)
                        {
                            ip = r->value; // NS address
                            break;
                        }
                    }
                }

                remaddr = (ip == "") ?  rootNsIp : ip;

                sleep_for(milliseconds(rand() % 100 + 100)); //delay random 100ms to 200ms
                udp.send(msg, remaddr);
                ++num_of_requests_sent;
            }
            else // a corresponding record was found in cache
            {
                // create a response packet, the fields 'qry_name' and 'qry_type' are unchanged
                dns.flags_response = 1;
                dns.flags_recavail = 0;
                dns.flags_recdesired = 0;
                dns.flags_rcode = 0;
                dns.count_answers = 1;
                dns.flags_authoritative = 0;
                dns.resp_name = dns.qry_name;
                dns.resp_type = 1;
                dns.resp_ttl = 120;

                // the NS responsible for a domain was asked for
                if(rec.type == "NS")
                    rec = *db.find(rec.value);

                dns.a = rec.value;

                msg = dns.toJson();

                sleep_for(milliseconds(rand() % 100 + 100)); //delay random 100ms to 200ms
                udp.send(msg, remaddr);

                num_of_responses_sent++;
//                dns.clear(); // clear dns packet before next query
            }

            now = time(0); // get current time
            logFile << now << " | " << remaddr << " | " << num_of_requests_sent << " | " << num_of_requests_received << " | " << num_of_responses_sent << " | " << num_of_responses_received << endl;
            debugFile << "Sent to " << remaddr << ": " << msg << endl;
cout << "Sent to " << remaddr << ": " << msg << endl;
        }
    }

    return 0;
}
