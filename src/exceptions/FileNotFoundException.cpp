//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException(const char *message) : _message(message) {
    Logger::log(Logger::LOG_LEVEL_ERROR, std::string(message));
}

const char *FileNotFoundException::what() const throw(){
    return  this->_message.c_str();
}
