#include "Logger.h"

void Logger::log(LogLevel level ,const HttpRequest &request, const HttpResponse &response){
    switch (level)
    {
        case LOG_LEVEL_DEBUG:
            std::cout << "DEBUG: " << FGRN()  << std::endl;
            break;
        case LOG_LEVEL_WARNING:
            std::cout << "WARNING: " << FRED() << std::endl;
            break;
        case LOG_LEVEL_ERROR:
            std::cout << "ERROR: " << FRED()  << std::endl;
            break;
    }
}