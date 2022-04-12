//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#ifndef WEBSERV_CONFIGPARSER_H
#define WEBSERV_CONFIGPARSER_H
#include "Server.h"
#include <vector>
#include <string>
#include <fstream>
#include "../container/BinaryTree.h"
#include "Token.h"
#include <stack>

class ConfigParser {
private:
    std::string configFilePath;
    std::ifstream configFile;
    std::string currentLine;
    int lastCursor;
    int cursor;
    int currentIndentation;
    int lastIndentation;
    std::stack<int> indentationStack;
    Node<Token *> *currentToken;
    BinaryTree<Node<Token* > *> ast; // Abstract Syntax Tree
public:
    ConfigParser(std::string configFilePath);
    std::vector<Server> parseConfigFiles(Node<Token *> *root, int lastIndentation, int currentIndentation);
    Node<Token *> *getNextToken(Token *token);
    int caluclateIndenetation();
};


#endif //WEBSERV_CONFIGPARSER_H
