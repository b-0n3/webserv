
#ifndef WEBSERV_FILENOTFOUNDEXCEPTION_H
#define WEBSERV_FILENOTFOUNDEXCEPTION_H

#include <string>
#include <exception>
#include "../tools/Logger.h"


class FileNotFoundException : virtual public std::exception {
private:
    std::string _message;
public:
    ~FileNotFoundException() throw() {}

    FileNotFoundException(const char *message);

    virtual const char *what() const throw();

};


#endif //WEBSERV_FILENOTFOUNDEXCEPTION_H
