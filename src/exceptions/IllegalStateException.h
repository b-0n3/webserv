//
// Created by Abdelouahad Ait hamd on 4/18/22.
//

#ifndef WEBSERV_ILLEGALSTATEEXCEPTION_H
#define WEBSERV_ILLEGALSTATEEXCEPTION_H
#include <string>

class IllegalStateException :virtual public std::exception {
private:
    std::string message;
public:
    virtual  ~IllegalStateException() throw() {};
    IllegalStateException(std::string message);
   virtual const char *what();
};


#endif //WEBSERV_ILLEGALSTATEEXCEPTION_H
