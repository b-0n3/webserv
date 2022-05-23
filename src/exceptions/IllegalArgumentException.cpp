//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include "IllegalArgumentException.h"

IllegalArgumentException::IllegalArgumentException(const std::string message) {
    this->message = message;
}

const char *IllegalArgumentException::what()   {
    return this->message.c_str();
}
