//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_ILLEGALARGUMENTEXCEPTION_H
#define WEBSERV_ILLEGALARGUMENTEXCEPTION_H
#include <string>
#include <exception>

class IllegalArgumentException:virtual public std::exception {
private :
    std::string message;
public:
  virtual  ~IllegalArgumentException() throw() {};
    IllegalArgumentException(const std::string &message);
   virtual const char* what() const throw() ;
};


#endif //WEBSERV_ILLEGALARGUMENTEXCEPTION_H
