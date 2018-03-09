//
// Created by jsszwc on 18-2-24.
//

#ifndef HTTPSERVER_SOCKETCTL_H
#define HTTPSERVER_SOCKETCTL_H

#include <unistd.h>
#include <netinet/in.h>

#include "netctl.h"
#include "logger.h"

struct SocketConn
{

    SocketConn(){}
    SocketConn(int fd, struct sockaddr_in addr) : m_fd(fd), m_ipAddr(addr) {}
    int setNonBlock(bool value = false);
    void close();

    int m_fd;
    IP4addr m_ipAddr;
};

struct SocketServer
{
    SocketServer(const std::string &host, int port):m_ipAddr(host, port)
    {
        LOG_DEBUG("%s", m_ipAddr.toString().c_str());
        socket();
        setReusePort(true);
        bind();
    }

    int listen(int backlog = 100);
    bool accept(SocketConn *conn);
    void close();

    int setReuseAddr(bool value = true);
    int setReusePort(bool value = true);
    int setNonBlock(bool value = false);

private:
    int socket();
    int bind();

    int m_fd;
    IP4addr m_ipAddr;
};

#endif //HTTPSERVER_SOCKETCTL_H
