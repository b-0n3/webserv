//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#ifndef WEBSERV_TOKEN_H
#define WEBSERV_TOKEN_H
#include <string>

typedef enum {

} TokenType;
class Token {
public:
    std::string value;
    TokenType type;
};


#endif //WEBSERV_TOKEN_H
