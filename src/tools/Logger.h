#include <iostream>
#include "../network/HttpRequest.h"
#include "../network/HttpResponse.h"
#include "Colors.h"

class   Logger
{
    public:
        enum LogLevel
        {
            LOG_LEVEL_DEBUG,
            LOG_LEVEL_WARNING,
            LOG_LEVEL_INFO,
            LOG_LEVEL_ERROR
        };
        static void log(LogLevel level,std::string message);
        static void log(LogLevel level , HttpRequest *request,  HttpResponse *response);
};