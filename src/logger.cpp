//
// Created by jsszwc on 18-2-24.
//

#include "logger.h"


Logger::Logger()
{
    m_count = 1;
    m_isAsync = true;
}
Logger::~Logger()
{
    if(m_fp)
        fclose(m_fp);
}

bool Logger::init(const char *logFilePath, LOGLEVEL loglevel, int64_t fileSizeLimit, bool isAsync)
{
    m_isAsync = isAsync;
    m_logFileSizeLimit = fileSizeLimit;
    m_loglevel = loglevel;

    time_t t = time(NULL);
    struct tm curTm = *localtime(&t);
    const char *p = strrchr(logFilePath, '/');
    char logFullName[1024];
    if(p == NULL)
    {
        m_logName = logFilePath;
        snprintf(logFullName, sizeof(logFullName), "%4d_%02d_%02d_%s",
                 curTm.tm_year+1900, curTm.tm_mon+1, curTm.tm_mday, m_logName.c_str());
    }
    else
    {
        m_logName = p+1;
        m_dirPath = std::string(logFilePath, p-logFilePath+1);
        snprintf(logFullName, sizeof(logFullName), "%s%4d_%02d_%02d_%s",
                 m_dirPath.c_str(), curTm.tm_year+1900, curTm.tm_mon+1, curTm.tm_mday, m_logName.c_str());
    }

    m_today = curTm.tm_mday;
    m_fp = fopen(logFullName, "a");
    if(!m_fp)
        return false;
    m_logFileSize = ftell(m_fp);
    return true;
}
void Logger::writeLog(LOGLEVEL logLevel, const char *format, ...)
{
    if(m_loglevel < logLevel)
        return;

    char str[20];
    switch (logLevel)
    {
        case ERROR: strcpy(str, "[error]:"); break;
        case WARN: strcpy(str, "[warn]:"); break;
        case INFO: strcpy(str, "[info]:"); break;
        case DEBUG: strcpy(str, "[debug]:"); break;
        default: strcpy(str, "[info]:"); break;
    }

    struct timeval curTime;
    memset(&curTime, 0, sizeof(curTime));
    gettimeofday(&curTime, NULL);
    time_t t = curTime.tv_sec;
    struct tm curTm = *localtime(&t);
    char logFullName[1024];

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if(curTm.tm_mday != m_today || m_logFileSize >= m_logFileSizeLimit)
        {
            fflush(m_fp);
            fclose(m_fp);

            char dateStr[50];
            snprintf(dateStr, sizeof(dateStr), "%4d_%02d_%02d_",
                     curTm.tm_year+1900, curTm.tm_mon+1, curTm.tm_mday);

            if(curTm.tm_mday != m_today)
            {
                snprintf(logFullName, sizeof(logFullName), "%s%s%s",
                         m_dirPath.c_str(), dateStr, m_logName.c_str());
                m_today = curTm.tm_mday;
                m_count = 1;
            }

            if(m_logFileSize >= m_logFileSizeLimit)
            {
                while(1)
                {
                    snprintf(logFullName, sizeof(logFullName), "%s%s%s.%d",
                             m_dirPath.c_str(), dateStr, m_logName.c_str(), m_count);

                    FILE *fp = fopen(logFullName, "r");
                    if(fp)
                    {
                        ++m_count;
                        fclose(fp);
                    }
                    else break;
                }
            }

            m_fp = fopen(logFullName, "a");
            m_logFileSize = 0;
        }
    }


    std::string logText;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        int bufSize = 10240;
        char *buf = new char[bufSize];
        va_list vlist;
        va_start(vlist, format);
        int n = snprintf(buf, 100, "%4d-%02d-%02d %02d:%02d:%02d:%06ld %s",
                                    curTm.tm_year+1900, curTm.tm_mon+1, curTm.tm_mday+1,
                                    curTm.tm_hour, curTm.tm_min, curTm.tm_sec, curTime.tv_usec, str);
        int m = vsnprintf(buf+n, bufSize-n, format, vlist);
        buf[n+m] = '\n';
        buf[n+m+1] = '\0';
        va_end(vlist);
        logText = buf;
        m_logFileSize += n+m;
        delete [] buf;
    }

    if(m_isAsync)
        m_logQueue.push(logText);
    else
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        fputs(logText.c_str(), m_fp);
        fflush(m_fp);
    }
}

void Logger::asyncWriteLog()
{
    bool flushFlag = false;
    std::string logText;

    while(1)
    {
        if(! m_logQueue.pop(logText, 100))
        {
            if(flushFlag)
            {
                flushFlag = false;
                fflush(m_fp);
            }
        }
        else
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            fputs(logText.c_str(), m_fp);
            flushFlag = true;
        }
    }
}