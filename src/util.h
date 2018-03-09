//
// Created by jsszwc on 18-2-24.
//

#ifndef HTTPSERVER_UTIL_H
#define HTTPSERVER_UTIL_H

#include <string>
#include <memory>
#include <cstdarg>

class Util
{
public:
    static std::string formatOutput(const char* format, ...);
};

#endif //HTTPSERVER_UTIL_H
