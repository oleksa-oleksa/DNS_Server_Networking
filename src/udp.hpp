#if (__OSX__) ||  (__APPLE__)
#define MSG_CONFIRM 0x800	/* Confirm path validity */
#endif

#ifndef _UDP_H_
#define _UDP_H_

#define UDPPORT 53053
#define BUFSIZE 4096
#define SOCKERR "Error: No socket - failed to create or bind\n"

#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

namespace udp
{
/*
* A UDP server/client
*/
class Udp
{
public:
    Udp(string ip, uint16_t port=UDPPORT):
        m_sock(-1), m_addrlen(sizeof(struct sockaddr_in))
    {
        // initialize remote address
        memset(&m_remaddr, 0, m_addrlen);
        m_remaddr.sin_family = AF_INET;

        // initialize local address
        memset(&m_locaddr, 0, m_addrlen);
        m_locaddr.sin_family = AF_INET;
        inet_aton(ip.c_str(), &m_locaddr.sin_addr);
        m_locaddr.sin_port = htons(port);

        // create and bind local socket
        if((m_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0)
            if((::bind(m_sock, (struct sockaddr*)&m_locaddr, m_addrlen)) < 0)
                m_sock = -1;
    }

    ~Udp()
    {
        close(m_sock);
    }

    /*
    * Receive packets
    *
    * Results are stored into
    * msg  - received message
    * ip   - remote IP address
    */
    void recv(string& msg, string& ip)
    {
        if(m_sock < 0)
            perror(SOCKERR);
        else
        {
            int n = recvfrom(m_sock, (char*)m_buf, sizeof(m_buf), MSG_WAITALL, (struct sockaddr*)&m_remaddr, &m_addrlen);

            // store received message
            m_buf[n] = '\0';
            msg = string(m_buf);

            // store remote address
            ip = string(inet_ntoa(m_remaddr.sin_addr));
        }
    }

    /*
    * Send packets
    *
    * msg  - message to send
    * ip   - remote IP address
    * port - remote port
    */
    void send(string msg, string ip, uint16_t port=UDPPORT)
    {
        if(m_sock < 0)
            perror(SOCKERR);
        else
        {
            // create remote destination address and port
            inet_pton(AF_INET, ip.c_str(), &(m_remaddr.sin_addr));
            m_remaddr.sin_port = htons(port);

            sendto(m_sock, msg.c_str(), msg.length()+1, MSG_CONFIRM, (struct sockaddr*)&m_remaddr, m_addrlen);
        }
    }

private:
    int m_sock;
    struct sockaddr_in m_locaddr, m_remaddr; // local and remote address
    socklen_t m_addrlen;
    char m_buf[BUFSIZE];
};

}

#endif
