//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#ifndef WEBSERV_TOKEN_H
#define WEBSERV_TOKEN_H
#include <string>

typedef enum {
    KEYWORD,
    VALUE,
    COMMENT,
    BEFOR_CECK,
    UNKNOWN
} TokenType;

class Token {
private:
    std::string value;
    TokenType type;
public:
    Token(std::string value, TokenType type);
    std::string getValue();
    TokenType getType();
    void setValue(std::string value);
    void setType(TokenType type);
};




#endif //WEBSERV_TOKEN_H
