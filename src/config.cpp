//
// Created by jsszwc on 18-2-24.
//

#include "config.h"

void Config::init()
{
    m_port = 61490;
    m_webPath = "./www/";
    m_logPath = "./log/hs.log";
    m_threadNum = 5;
    m_backlog = 2000;
    m_maxEvent = 3000;
    m_logLevel = 1;
    m_logSize = 50 * 1024 * 1024;
    m_waitMs = -1;
}
void Config::show()
{
    printf("PORT: %d\n", m_port);
    printf("WEBPATH: %s\n", m_webPath.c_str());
    printf("LOGPATH: %s\n", m_logPath.c_str());
    printf("THREADNUM: %d\n", m_threadNum);
    printf("BACKLOG: %d\n", m_backlog);
    printf("MAXEVENT: %d\n", m_maxEvent);
    printf("LOGLEVEL: %d\n", m_logLevel);
    printf("LOGSIZE: %dM\n", m_logSize/1024/1024);
    printf("WAITMS: %d\n", m_waitMs);
}
bool Config::read(const char *configFilePath)
{
    init();

    FILE *fp = fopen(configFilePath, "r");
    if(!fp)
        return false;

    char buf[1024];
    while(fgets(buf, sizeof(buf), fp))
    {
        if(strchr(buf, '#')) continue;

        char *ptr = strchr(buf, '=');
        if(!ptr) continue;

        *ptr = '\0';
        ++ptr;
        if(strcmp(buf, "PORT") == 0)
        {
            m_port = atoi(ptr);
        }
        else if(strcmp(buf, "WEBPATH") == 0)
        {
            for(int i = 0; ptr[i]; ++i)
                if(isspace(ptr[i]))
                {
                    ptr[i] = '\0';
                    break;
                }
            m_webPath = ptr;
        }
        else if(strcmp(buf, "LOGPATH") == 0)
        {
            for(int i = 0; ptr[i]; ++i)
                if(isspace(ptr[i]))
                {
                    ptr[i] = '\0';
                    break;
                }
            m_logPath = ptr;
        }

        else if(strcmp(buf, "THREADNUM") == 0)
        {
            m_threadNum = atoi(ptr);
        }
        else if(strcmp(buf, "BACKLOG") == 0)
        {
            m_backlog = atoi(ptr);
        }
        else if(strcmp(buf, "MAXEVENT") == 0)
        {
            m_maxEvent = atoi(ptr);
        }
        else if(strcmp(buf, "LOGLEVEL") == 0)
        {
            m_logLevel = atoi(ptr);
        }
        else if(strcmp(buf, "LOGSIZE") == 0)
        {
            m_logSize = atoi(ptr);
        }
        else if(strcmp(buf, "WAITMS") == 0)
        {
            m_waitMs = atoi(ptr);
        }
    }

    if(ferror(fp))
        return false;

    fclose(fp);
    return true;
}
