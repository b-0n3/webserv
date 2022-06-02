

#include "IllegalArgumentException.h"
#include "../tools/Logger.h"

IllegalArgumentException::IllegalArgumentException(const std::string &message) {
    this->message = message;
    Logger::log(Logger::LOG_LEVEL_ERROR, message);
}

const char *IllegalArgumentException::what() const  throw()   {
    return this->message.c_str();
}
