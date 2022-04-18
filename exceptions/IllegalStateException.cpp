//
// Created by Abdelouahad Ait hamd on 4/18/22.
//

#include "IllegalStateException.h"

IllegalStateException::IllegalStateException(std::string message) {
    this->message = message;
}

const char *IllegalStateException::what(){
    return message.c_str();
}
