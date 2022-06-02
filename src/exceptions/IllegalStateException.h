

#ifndef WEBSERV_ILLEGALSTATEEXCEPTION_H
#define WEBSERV_ILLEGALSTATEEXCEPTION_H

#include <string>
#include <exception>

class IllegalStateException : virtual public std::exception {
private:
    std::string message;
public:
    virtual  ~IllegalStateException() throw() {};

    IllegalStateException(std::string message);

    virtual const char *what() const throw();
};


#endif //WEBSERV_ILLEGALSTATEEXCEPTION_H
