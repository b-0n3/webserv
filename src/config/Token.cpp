

#include "Token.h"

TokenType Token::getType() {
    return this->type;
}

std::string Token::getValue() {
    return this->value;
}

Token::Token(std::string value, TokenType type) {
    this->value = value;
    this->type = type;
    this->indentation = -1;

}

void Token::setValue(std::string value) {
    this->value = value;
}

void Token::setType(TokenType type) {
    this->type = type;
}
