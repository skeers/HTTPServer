//
// Created by jsszwc on 18-2-25.
//

#include "poller.h"

Poller::Poller()
{
    m_epollFd = epoll_create1(0);
    if(m_epollFd < 0)
    {
        LOG_ERROR("epoll_create failed: errno %d", errno);
        LOG_WAIT(2);
        exit(-1);
    }
    m_activeEvent = new epoll_event[conf.m_maxEvent];
}
Poller::~Poller()
{
    if(m_activeEvent)
        delete [] m_activeEvent;
}

void Poller::loop()
{
    while(1)
        loopWork(conf.m_waitMs);
}
void Poller::loopWork(int waitMs)
{
    int num = epoll_wait(m_epollFd, m_activeEvent, conf.m_maxEvent, waitMs);
    for(int i = 0; i < num; ++i)
    {
        HTTPConn *conn = (HTTPConn*)(m_activeEvent[i].data.ptr);
        int event = m_activeEvent[i].events;
        if(event & EPOLLIN)
        {
            if(conn->read() < 0)
            {
                httpConnQueue.push(conn);
                LOG_INFO("client close: %s", conn->m_sockConn.m_ipAddr.toString().c_str());
            }
        }
        else if(event & EPOLLOUT)
        {
            if(conn->write() < 0)
            {
                httpConnQueue.push(conn);
                LOG_INFO("client close: %s", conn->m_sockConn.m_ipAddr.toString().c_str());
            }
        }
        else
            LOG_WARN("unknow event %d", event);
    }
}
int Poller::addHTTPConn(HTTPConn *conn, int event)
{
    return addEpollEvent(conn->m_sockConn.m_fd, conn, event);
}
int Poller::modHTTPConn(HTTPConn *conn, int event)
{
    return modEpollEvent(conn->m_sockConn.m_fd, conn, event);
}
int Poller::delHTTPConn(HTTPConn *conn)
{
    return delEpollEvent(conn->m_sockConn.m_fd, conn, EPOLLIN);
}
int Poller::addEpollEvent(int fd, void *ptr, int event)
{
    struct epoll_event epollEvent;
    epollEvent.events = event;
    epollEvent.data.ptr = ptr;
    epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &epollEvent);
}
int Poller::modEpollEvent(int fd, void *ptr, int event)
{
    struct epoll_event epollEvent;
    epollEvent.events = event;
    epollEvent.data.ptr = ptr;
    epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &epollEvent);
}
int Poller::delEpollEvent(int fd, void *ptr, int event)
{
    struct epoll_event epollEvent;
    epollEvent.events = event;
    epollEvent.data.ptr = ptr;
    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &epollEvent);
}