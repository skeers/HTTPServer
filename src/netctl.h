//
// Created by jsszwc on 18-2-24.
//

#ifndef HTTPSERVER_NETCTL_H
#define HTTPSERVER_NETCTL_H

#include <string>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <cstring>
#include "util.h"

class Net
{
public:
    static int setNonBlock(int fd, bool value = true);
    static int setReuseAddr(int fd, bool value = true);
    static int setReusePort(int fd, bool value = true);

    static struct in_addr getHostByName(const std::string &host);
};

struct IP4addr
{
    IP4addr(){}
    IP4addr(const std::string &host, uint16_t port);
    IP4addr(uint16_t port) : IP4addr("", port){}
    IP4addr(const struct sockaddr_in &addr) : m_addr(addr) {}

    std::string toString();
    std::string ipString();
    uint16_t getPort();
    uint32_t getIpInt();

    struct sockaddr_in m_addr;
};

#endif //HTTPSERVER_NETCTL_H
