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
#include "../tools/Utils.h"

class ConfigParser {
private:
    std::string configFilePath;
    std::ifstream *configFile;
    std::string currentLine;
    int lastCursor;
    int cursor;
    int currentIndentation;
    int lastIndentation;
    std::stack<int> indentationStack;
    Node<Token *> *currentToken;
     // Abstract Syntax Tree
public:
    BinaryTree<Token*> ast;
    ConfigParser(std::string configFilePath);
    void  tokenizeConfigFiles(Node<Token *> *parent, Node<Token *> *root, int lastIndentation, int currentIndentation);
    Node<Token *> *getNextToken();
    int caluclateIndenetation();
    std::vector<Server *> validateAst();
};


#endif //WEBSERV_CONFIGPARSER_H
