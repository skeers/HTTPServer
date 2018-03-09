//
// Created by jsszwc on 18-2-24.
//

#ifndef HTTPSERVER_LOGGER_H
#define HTTPSERVER_LOGGER_H

#include <cstdint>
#include <cstdio>
#include <string>
#include <cstring>
#include <cctype>
#include <ctime>
#include <mutex>
#include <sys/time.h>
#include <cstdarg>

#include "squeue.h"


class Logger
{
public:
    enum LOGLEVEL{OFF, ERROR, WARN, INFO, DEBUG, ALL};

    static Logger* getInstance()
    {
        static Logger instance;
        return &instance;
    }

    static void flushLogThread()
    {
        getInstance()->asyncWriteLog();
    }

    bool init(const char *logFilePath, LOGLEVEL loglevel, int64_t fileSizeLimit, bool isAsync);
    void writeLog(LOGLEVEL logLevel, const char *format, ...);

private:

    Logger();
    ~Logger();
    void asyncWriteLog();

private:
    bool m_isAsync;
    std::mutex m_mutex;
    int64_t m_logFileSizeLimit;
    int64_t m_logFileSize;
    std::string m_dirPath;
    std::string m_logName;
    int m_count;
    int m_today;
    LOGLEVEL m_loglevel;
    FILE *m_fp;
    Squeue<std::string> m_logQueue;
};

#define LOG_ERROR(format, ...) Logger::getInstance()->writeLog(Logger::ERROR, format, __VA_ARGS__)
#define LOG_WARN(format, ...) Logger::getInstance()->writeLog(Logger::WARN, format, __VA_ARGS__)
#define LOG_INFO(format, ...) Logger::getInstance()->writeLog(Logger::INFO, format, __VA_ARGS__)
#define LOG_DEBUG(format, ...) Logger::getInstance()->writeLog(Logger::DEBUG, format, __VA_ARGS__)
#define LOG_WAIT(s) std::this_thread::sleep_for(std::chrono::seconds(s))


#endif //HTTPSERVER_LOGGER_H
