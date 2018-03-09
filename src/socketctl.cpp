//
// Created by jsszwc on 18-2-24.
//

#include "socketctl.h"

int SocketConn::setNonBlock(bool value)
{
    return Net::setNonBlock(m_fd, value);
}

void SocketConn::close()
{
    if(m_fd != -1)
        ::close(m_fd);
    m_fd = -1;
}

int SocketServer::setNonBlock(bool value)
{
    return Net::setNonBlock(m_fd, value);
}

int SocketServer::setReusePort(bool value)
{
    return Net::setReusePort(m_fd, value);
}

int SocketServer::setReuseAddr(bool value)
{
    return Net::setReuseAddr(m_fd, value);
}

int SocketServer::socket()
{
    m_fd = ::socket(PF_INET, SOCK_STREAM, 0);
    if(m_fd < 0)
    {
        LOG_ERROR("socket failed: errno %d", errno);
        exit(-1);
    }
    return m_fd;
}
int SocketServer::bind()
{
    struct sockaddr_in &addr = m_ipAddr.m_addr;
    int val = ::bind(m_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(val < 0)
    {
        LOG_ERROR("bind failed: errno %d", errno);
        exit(-1);
    }
    return val;
}
int SocketServer::listen(int backlog)
{
    int val = ::listen(m_fd, backlog);
    if(val < 0)
    {
        LOG_ERROR("listen failed: errno %d", errno);
        exit(-1);
    }
    return val;
}

bool SocketServer::accept(SocketConn *conn)
{
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientFd = ::accept(m_fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if(clientFd < 0)
    {
        LOG_ERROR("accept conn failed: errno %d", errno);
        exit(-1);
    }

    conn->m_fd = clientFd;
    conn->m_ipAddr.m_addr = clientAddr;
    return true;
}

void SocketServer::close()
{
    if(m_fd != -1)
        ::close(m_fd);
    m_fd = -1;
}
