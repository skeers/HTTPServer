//
// Created by jsszwc on 18-2-25.
//

#ifndef HTTPSERVER_POLLER_H
#define HTTPSERVER_POLLER_H

#include <sys/epoll.h>
#include <thread>
#include "logger.h"
#include "http.h"
#include "config.h"

extern Config conf;
extern Squeue<HTTPConn*> httpConnQueue;


class Poller
{
public:
    Poller();
    ~Poller();

    void loop();
    void loopWork(int waitMs);

    int addHTTPConn(HTTPConn *conn, int event);
    int modHTTPConn(HTTPConn *conn, int event);
    int delHTTPConn(HTTPConn *conn);

    int addEpollEvent(int fd, void *ptr, int event);
    int modEpollEvent(int fd, void *ptr, int event);
    int delEpollEvent(int fd, void *ptr, int event);

private:
    int m_epollFd;
    struct epoll_event *m_activeEvent;
};

#endif //HTTPSERVER_POLLER_H
