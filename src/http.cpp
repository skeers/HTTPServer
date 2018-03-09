//
// Created by jsszwc on 18-2-25.
//

#include "http.h"
#include "poller.h"


int HTTPConn::read()
{
    char buf[4096];
    int len = 0;
    if((len = ::read(m_sockConn.m_fd, buf, sizeof(buf)-1)) > 0)
    {
        buf[len] = '\0';
        parserContent(buf, len);

        poller->modHTTPConn(this, EPOLLOUT);
        return 0;
    }
    else if(len < 0)
        LOG_ERROR("read HTTPReques failed: errno %d", errno);

    poller->delHTTPConn(this);
    close();
    return -1;
}

int HTTPConn::write()
{
    int contentLength = 0;
    if(m_req.method != "GET")
    {
        m_res.status_code = 501;
        m_res.reason = "Not Implemented";
    }
    else if(m_req.path.find("../") != std::string::npos)
    {
        m_res.status_code = 400;
        m_res.reason = "Bad Request";
    }
    else
    {
        struct stat st;
        if(stat(m_req.path.c_str(), &st) < 0)
        {
            m_res.status_code = 404;
            m_res.reason = "Not Found";
        }
        else if(!(S_ISREG(st.st_mode)) || !(S_IRUSR & st.st_mode))
        {
            m_res.status_code = 400;
            m_res.reason = "Bad Request";
        }
        else
        {
            m_res.status_code = 200;
            m_res.reason = "OK";
            contentLength = st.st_size;
        }
    }

    char header[1024];
    if(m_req.keep_alive)
        snprintf(header, sizeof(header), KEEP_ALIVE_HEADERS, m_res.status_code, m_res.reason.c_str(), contentLength);
    else
        snprintf(header, sizeof(header), NO_KEEP_ALIVE_HEADERS, m_res.status_code, m_res.reason.c_str(), contentLength);

    if(::write(m_sockConn.m_fd, header, strlen(header)) < 0)
    {
        LOG_ERROR("write failed: errno %d", errno);
        poller->delHTTPConn(this);
        close();
        return -1;
    }
    if(contentLength > 0)
    {
        int fd = ::open(m_req.path.c_str(), O_RDONLY, 0);
        sendfile(m_sockConn.m_fd, fd, 0, contentLength);
        ::close(fd);
    }

    if(m_req.keep_alive)
    {
        poller->modHTTPConn(this, EPOLLIN);
        return 0;
    }
    else
    {
        poller->delHTTPConn(this);
        close();
        return -1;
    }
}
void HTTPConn::close()
{
    m_sockConn.close();
}
void HTTPConn::parserContent(char *buf, int len)
{
    char *ptr = NULL;
    if((ptr = strstr(buf, "\r\n\r\n")) == NULL)
        return;
    *ptr = '\0';

    int i = 0;
    while(buf[i] != ' ')
    {
        buf[i] = toupper(buf[i]);
        ++i;
    }
    buf[i] = '\0';
    m_req.method = buf;

    ++i;
    char *path = buf + i;
    while(buf[i] != ' ')
        ++i;
    buf[i] = '\0';
    m_req.path = path;
    m_req.path = conf.m_webPath + m_req.path;

    ++i;
    for(int j = i; j < len; ++j)
        buf[j] = tolower(buf[j]);
    if(strstr(buf+i, "keep-alive"))
        m_req.keep_alive = true;
    else
        m_req.keep_alive = false;
    LOG_DEBUG("%s", buf+i);
    LOG_DEBUG("method: %s", m_req.method.c_str());
    LOG_DEBUG("path: %s", m_req.path.c_str());
}

int HTTPConn::setNonBlock(bool value)
{
    return m_sockConn.setNonBlock(value);
}

int HTTPServer::start(int backlog)
{
    return m_sockServer.listen(backlog);
}
bool HTTPServer::accept(HTTPConn *conn)
{
    return m_sockServer.accept(&(conn->m_sockConn));
}

