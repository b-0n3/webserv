//
// Created by Abdelouahad Ait hamd on 4/18/22.
//

#include "IllegalStateException.h"
#include "../tools/Logger.h"

IllegalStateException::IllegalStateException(std::string message) {
    this->message = message;
    Logger::log(Logger::LOG_LEVEL_ERROR, message);
}

const char *IllegalStateException::what() const throw(){
    return message.c_str();
}
