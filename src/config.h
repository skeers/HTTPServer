//
// Created by jsszwc on 18-2-24.
//

#ifndef HTTPSERVER_CONFIG_H
#define HTTPSERVER_CONFIG_H

#include <cstdint>
#include <string>
#include <cstdio>
#include <cstring>
#include <cctype>

struct Config
{
    void init();
    bool read(const char *configFilePath);
    void show();

    uint16_t m_port;
    std::string m_webPath;
    std::string m_logPath;
    int m_threadNum;
    int m_backlog;
    int m_maxEvent;
    int m_logLevel;
    int m_logSize;
    int m_waitMs;
};

#endif //HTTPSERVER_CONFIG_H
