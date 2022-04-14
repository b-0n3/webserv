//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

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
    std::string getValue() const;
    TokenType getType();
    void setValue(std::string value);
    void setType(TokenType type);
};

std::ostream &operator<<(std::ostream &os, const Token &token);



#endif //WEBSERV_TOKEN_H
