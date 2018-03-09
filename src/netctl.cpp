//
// Created by jsszwc on 18-2-24.
//

#include "netctl.h"

int Net::setNonBlock(int fd, bool value)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag < 0)
        return -1;

    if(value)
        return fcntl(fd, F_SETFL, flag | O_NONBLOCK);
    else
        return fcntl(fd, F_SETFL, flag & ~O_NONBLOCK);
}

int Net::setReuseAddr(int fd, bool value)
{
    int flag = value;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
}

int Net::setReusePort(int fd, bool value)
{
    int flag = value;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
}

struct in_addr Net::getHostByName(const std::string &host)
{
    struct in_addr addr;

    if(host.size() == 0)
    {
        addr.s_addr = INADDR_NONE;
        return addr;
    }

    struct hostent hent;
    struct hostent *he;
    char buf[1024];
    int herrno;
    memset(&hent, 0, sizeof(hent));
    memset(buf, 0, sizeof(buf));
    int r = gethostbyname_r(host.c_str(), &hent, buf, sizeof(buf), &he, &herrno);
    if(r == 0 && he && he->h_addrtype == AF_INET)
        addr = *(struct in_addr*)(he->h_addr);
    else
        addr.s_addr = INADDR_NONE;
    return addr;
}

IP4addr::IP4addr(const std::string &host, uint16_t port)
{
    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(port);
    m_addr.sin_addr = Net::getHostByName(host);

    if(m_addr.sin_addr.s_addr == INADDR_NONE)
        m_addr.sin_addr.s_addr = INADDR_ANY;
}

std::string IP4addr::toString()
{
    uint32_t uip = m_addr.sin_addr.s_addr;
    return Util::formatOutput("%d.%d.%d.%d:%d",
                              (uip>>0)&0xff, (uip>>8)&0xff, (uip>>16)&0xff, (uip>>24)&0xff,
                              ntohs(m_addr.sin_port));
}
std::string IP4addr::ipString()
{
    uint32_t uip = m_addr.sin_addr.s_addr;
    return Util::formatOutput("%d.%d.%d.%d",
                              (uip>>0)&0xff, (uip>>8)&0xff, (uip>>16)&0xff, (uip>>24)&0xff);
}

uint16_t IP4addr::getPort()
{
    return ntohs(m_addr.sin_port);
}
uint32_t IP4addr::getIpInt()
{
    return ntohl(m_addr.sin_addr.s_addr);
}

