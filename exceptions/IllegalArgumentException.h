//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_ILLEGALARGUMENTEXCEPTION_H
#define WEBSERV_ILLEGALARGUMENTEXCEPTION_H
#include <string>

class IllegalArgumentException: public std::exception {
private :
    std::string message;
public:
    IllegalArgumentException(const std::stirng message);
    const char* what() const ;
};


#endif //WEBSERV_ILLEGALARGUMENTEXCEPTION_H
