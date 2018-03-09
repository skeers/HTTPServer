//
// Created by jsszwc on 18-2-24.
//

#include "util.h"

std::string Util::formatOutput(const char *format, ...)
{
    int bufSize = 1024;
    char *buf = new char[bufSize];
    std::unique_ptr<char[]> uptr;
    uptr.reset(buf);

    va_list vlist;
    va_start(vlist, format);
    int len = vsnprintf(buf, bufSize, format, vlist);
    va_end(vlist);

    if(len >= bufSize)
    {
        bufSize = len + 10;
        buf = new char[bufSize];
        uptr.reset(buf);

        va_list vlist;
        va_start(vlist, format);
        vsnprintf(buf, bufSize, format, vlist);
        va_end(vlist);
    }

    buf[len] = '\0';
    return std::string(buf);
}
