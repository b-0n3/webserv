//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_FILENOTFOUNDEXCEPTION_H
#define WEBSERV_FILENOTFOUNDEXCEPTION_H
#include <string>


class FileNotFoundException : public std::exception {
private:
    std::string _message;
public:
    ~FileNotFoundException() throw() {}
    FileNotFoundException(const char *message);
    const char *what();

};


#endif //WEBSERV_FILENOTFOUNDEXCEPTION_H
