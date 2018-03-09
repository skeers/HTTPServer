//
// Created by jsszwc on 18-2-25.
//

#ifndef HTTPSERVER_HTTP_H
#define HTTPSERVER_HTTP_H


#include <string>
#include <cctype>
#include <cstring>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include "imp.h"
#include "socketctl.h"
#include "config.h"

extern Config conf;

#define KEEP_ALIVE_HEADERS "HTTP/1.1 %d %s\r\n\
Server: JSSZWC/0.1\r\n\
Connection: Keep-Alive\r\n\
Keep-Alive: timeout=20\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
\r\n"

#define NO_KEEP_ALIVE_HEADERS "HTTP/1.1 %d %s\r\n\
Server: JSSZWC/0.1\r\n\
Connection: close\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
\r\n"

struct HTTPRequest
{
    std::string method, path;
    bool keep_alive;
};
struct HTTPResponse
{
    int status_code;
    std::string reason;
};

struct HTTPConn
{
    HTTPConn(){}
    HTTPConn(const SocketConn &sockConn) : m_sockConn(sockConn) {}
    HTTPConn(int fd, sockaddr_in addr) : m_sockConn(fd, addr) {}
    int read();
    int write();
    void close();
    int setNonBlock(bool value = false);

    HTTPRequest m_req;
    HTTPResponse m_res;
    SocketConn m_sockConn;
    Poller *poller;

private:
    void parserContent(char *buf, int len);

};

struct HTTPServer
{
    HTTPServer(const std::string &host, uint16_t port):m_sockServer(host, port) {}
    int start(int backlog);
    bool accept(HTTPConn *conn);
    void close();

    SocketServer m_sockServer;
};


#endif //HTTPSERVER_HTTP_H
