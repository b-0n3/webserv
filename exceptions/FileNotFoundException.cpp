//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException(const char *message) : _message(message) {}

const char *FileNotFoundException::what() {
    return  this->_message.c_str();
}
