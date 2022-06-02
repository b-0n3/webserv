#include "Logger.h"

void Logger::log(LogLevel level , HttpRequest *request,  HttpResponse *response){
    std::string message = request->log() + response->log();
    switch (level)
    {

        case LOG_LEVEL_DEBUG:
            std::cout << "DEBUG: " << FBLU(message)  << std::endl;
            break;
        case LOG_LEVEL_WARNING:
            std::cout << "WARNING: " << FYEL(message) << std::endl;
            break;
        case LOG_LEVEL_ERROR:
            std::cout << "ERROR: " << FRED(message)  << std::endl;
            break;
        case LOG_LEVEL_INFO:
            std::cout << "INFO: " << FGRN(message)  << std::endl;
            break;
    }
}

void Logger::log(Logger::LogLevel level, std::string message) {
    switch (level)
    {
        case LOG_LEVEL_DEBUG:
            std::cout << "DEBUG: " << FBLU(message)  << std::endl;
            break;
        case LOG_LEVEL_WARNING:
            std::cout << "WARNING: " << FYEL(message) << std::endl;
            break;
        case LOG_LEVEL_ERROR:
            std::cout << "ERROR: " << FRED(message)  << std::endl;
            break;
        case LOG_LEVEL_INFO:
            std::cout << "INFO: " << FGRN(message)  << std::endl;
            break;
    }
}
