/*
* dnspacket.h
*
* DNS Packet Interface
*
* Oleksandra Baga
* Milos Budimir
* Jannes Volkens
* Michael Zent
*
* Telematik, Prof. M. Wählisch
* WS 2020/21 FU Berlin
*/

#ifndef _DNS_PACKET_H
#define _DNS_PACKET_H

#include <iostream>
#include <json/json.h>
#include <sstream>

using namespace std;

/*
*/
class DnsPacket
{
public:
    DnsPacket():
        a(""), ns(""),
        count_answers(0), count_auth_rr(0), count_add_rr(0),
        flags_authoritative(false), flags_rcode(0), flags_recdesired(false), flags_recavail(false), flags_response(false),
        qry_name(""), qry_type(0), resp_name(""), resp_type(0), resp_ttl(0), resp_ttl_a(0), resp_ttl_ns(0),
        srv_name(""), srv_port(0), srv_proto(""), srv_service(""), srv_target("")
    {
    }

    ~DnsPacket()
    {
    }

    /*
    * Set back all packet fields
    */
    void clear(void)
    {
        a = "";
        ns = "";
        count_answers = 0;
        count_auth_rr = 0;
        count_add_rr = 0;
        flags_authoritative = false;
        flags_rcode = 0;
        flags_recdesired = false;
        flags_recavail = false;
        flags_response = false;
        qry_name = "";
        qry_type = 0;
        resp_name = "";
        resp_type = 0;
        resp_ttl = 0;
        resp_ttl_a = 0;
        resp_ttl_ns = 0;
        srv_name = "";
        srv_port = 0;
        srv_proto = "";
        srv_service = "";
        srv_target = "";
    }

    /*
    * Query to resolve the given name. Returns an JSON string.
    */
    string query(string name, string type)
    {
        clear();

        if(type == "A")
            qry_type = 1; // address
        else if(type == "NS")
            qry_type = 2; // name server
        else
            return "";

        qry_name = name + ".";
        flags_recdesired = 1;

        return toJson();
    }

    /*
    * Query to resolve the given name. Returns an JSON string.
    */
/*
    string response(DnsRecord* rec)
    {
        clear();

        flags_response = 1;
    }
*/
    /*
    * Read the data from an JSON string into the packet structure
    */
    void fromJson(string& str)
    {
        Json::Value obj;
        Json::Reader reader;

        if(!reader.parse(str, obj))
        {
           cout << reader.getFormattedErrorMessages();
           return;
        }

        a = obj["dns.a"].asString();
        ns = obj["dns.ns"].asString();
        count_answers = (uint16_t)obj["dns.count.answers"].asUInt();
        count_auth_rr = (uint16_t)obj["dns.count.auth_rr"].asUInt();
        count_add_rr = (uint16_t)obj["dns.count.add_rr"].asUInt();
        flags_authoritative = obj["dns.flags.authoritative"].asBool();
        flags_rcode = (uint16_t)obj["dns.flags.rcode"].asUInt(); // reply code
        flags_recdesired = obj["dns.flags.recdesired"].asBool(); // recursion desired
        flags_recavail = obj["dns.flags.recavail"].asBool(); // recursion available
        flags_response = obj["dns.flags.response"].asBool();
        qry_name = obj["dns.qry.name"].asString();
        qry_type = (uint16_t)obj["dns.qry.type"].asUInt();
        resp_name = obj["dns.resp.name"].asString();
        resp_type = obj["dns.resp.type"].asUInt();
        resp_ttl = obj["dns.resp.ttl"].asUInt();
        resp_ttl_a = obj["dns.resp.ttl.a"].asUInt();
        resp_ttl_ns = obj["dns.resp.ttl.ns"].asUInt();
        srv_name = obj["dns.srv.name"].asString();
        srv_port = (uint16_t)obj["dns.srv.port"].asUInt();;
        srv_proto = obj["dns.srv.proto"].asString();
        srv_service = obj["dns.srv.service"].asString();
        srv_target = obj["dns.srv.target"].asString();
    }

    /*
    * Read the data from the packet structure into an JSON string
    */
    string toJson(void)
    {
        ostringstream ostr(ios::app);
        ostr
            << "{\n"
            << "  \"dns.a\" : \"" << a << "\",\n"
            << "  \"dns.ns\" : \"" << ns << "\",\n"
            << "  \"dns.count.answers\" : " << count_answers << ",\n"
            << "  \"dns.count.auth_rr\" : " << count_auth_rr << ",\n"
            << "  \"dns.count.add_rr\" : " << count_add_rr << ",\n"
            << "  \"dns.flags.authoritative\" : " << flags_authoritative << ",\n"
            << "  \"dns.flags.rcode\" : " << flags_rcode << ",\n"
            << "  \"dns.flags.recdesired\" : " << flags_recdesired << ",\n"
            << "  \"dns.flags.recavail\" : " << flags_recavail << ",\n"
            << "  \"dns.flags.response\" : " << flags_response << ",\n"
            << "  \"dns.qry.name\" : \"" << qry_name << "\",\n"
            << "  \"dns.qry.type\" : " << qry_type << ",\n"
            << "  \"dns.resp.name\" : \"" << resp_name << "\",\n"
            << "  \"dns.resp.type\" : " << resp_type << ",\n"
            << "  \"dns.resp.ttl\" : " << resp_ttl << ",\n"
            << "  \"dns.resp.ttl.a\" : " << resp_ttl_a << ",\n"
            << "  \"dns.resp.ttl.ns\" : " << resp_ttl_ns << ",\n"
            << "  \"dns.srv.name\" : \"" << srv_name << "\",\n"
            << "  \"dns.srv.port\" : " << srv_port << ",\n"
            << "  \"dns.srv.proto\" : \"" << srv_proto << "\",\n"
            << "  \"dns.srv.service\" : \"" << srv_service << "\",\n"
            << "  \"dns.srv.target\" : \"" << srv_target << "\"\n"
            << "}\n";
        return ostr.str();
    }

    string a;
    string ns;
    uint16_t count_answers;
    uint16_t count_auth_rr;
    uint16_t count_add_rr;
    bool flags_authoritative;
    uint16_t flags_rcode; // reply code
    bool flags_recdesired; // recursion desired
    bool flags_recavail; // recursion available
    bool flags_response;
    string qry_name;
    uint16_t qry_type; // 1 - A (address), 2 - NS (name server)
    string resp_name;
    uint16_t resp_type; // 1 - A (address), 2 - NS (name server)
    uint32_t resp_ttl;
    uint32_t resp_ttl_a;
    uint32_t resp_ttl_ns;
    string srv_name;
    uint16_t srv_port;
    string srv_proto;
    string srv_service;
    string srv_target;
};

#endif /*_DNS_PACKET_H */
