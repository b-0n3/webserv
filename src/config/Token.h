
#ifndef WEBSERV_TOKEN_H
#define WEBSERV_TOKEN_H

#include <iostream>

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
    int indentation;

    Token(std::string value, TokenType type);

    std::string getValue();

    TokenType getType();

    void setValue(std::string value);

    void setType(TokenType type);
};


#endif //WEBSERV_TOKEN_H
