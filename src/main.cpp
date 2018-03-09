#include <iostream>
#include <thread>
#include <vector>


#include "config.h"
#include "logger.h"
#include "http.h"
#include "poller.h"


Config conf;
Squeue<HTTPConn*> httpConnQueue;

void initLog();

int main(int argc, char *argv[])
{
    const char *configFilePath = "./httpConfig.conf";

    if(argc > 1)
        configFilePath = argv[1];

    if(conf.read(configFilePath) < 0)
    {
        printf("read http config failed\n");
        exit(-1);
    }

    conf.show();
    initLog();

    HTTPServer server("0.0.0.0", conf.m_port);
    server.start(conf.m_backlog);

    Poller poller[conf.m_threadNum];
    std::vector<std::thread> threads;
    for(int i = 0; i < conf.m_threadNum; ++i)
        threads.emplace_back(&Poller::loop, std::ref(poller[i]));

    while(1)
    {
        for(int i = 0; i < conf.m_threadNum; )
        {
            HTTPConn *conn = NULL;
            if(! httpConnQueue.pop(conn, 100))
                conn = new HTTPConn;
            if(!server.accept(conn))
            {
                LOG_ERROR("HTTPConn accpet failed: errno %d", errno);
                continue;
            }

            if(! conn) continue;
            conn->setNonBlock(true);
            conn->poller = &poller[i];
            poller[i].addHTTPConn(conn, EPOLLIN);

            ++i;
        }
    }

    return 0;
}

void initLog()
{
    if(conf.m_logLevel < 0 || conf.m_logLevel > 5)
        conf.m_logLevel = 1;
    Logger::LOGLEVEL loglevel = (Logger::LOGLEVEL)(conf.m_logLevel);
    if(Logger::getInstance()->init(conf.m_logPath.c_str(), loglevel, conf.m_logSize*1024*1024, true))
    {
        std::thread logThread([](){
            Logger::flushLogThread();
        });
        logThread.detach();
    }
    else
    {
        printf("log init failed\n");
        exit(-1);
    }
}