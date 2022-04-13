//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#include "ConfigParser.h"
#include "../exceptions/FileNotFoundException.h"


ConfigParser::ConfigParser(std::string configFilePath) {
    std::ifstream *file = new std::ifstream (configFilePath);
    if (!file->is_open()) {
        throw FileNotFoundException("Config file not found");
    }
    this->configFile = file;
    this->configFilePath  = configFilePath;
    this->currentIndentation = -1;
    this->lastIndentation = -1;
}
bool isEmptyLine(std::string &line)
{
    if (line.find_first_not_of(" \t") == std::string::npos)
        return true;
    return false;
}
bool isOnlyComment(std::string &line)
{
  int i = 0;
  while (line[i] == ' ' || line[i] == '\t')
    i++;
  if (line[i] == '#')
    return true;
  return false;
}
bool isArrayElment(std::string &line)
{
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t')
        i++;
    if (line[i] == '-')
        return true;
    return false;
}

void ConfigParser::tokenizeConfigFiles(Node<Token *> *root, int lastIndentation, int currentIndentation) {
    int indentation;
    Node<Token *> *currentNode = root;
    if (std::getline(*this->configFile, this->currentLine).eof())
        return;// @todo:  Change and call other function to convert ast into a vector of server;// @todo:  Change and call other function to convert ast into a vector of server
    //std::cout << this->currentLine<<std::endl;
    if  (isEmptyLine(this->currentLine) || isOnlyComment(this->currentLine)) {

        return this->tokenizeConfigFiles(root, lastIndentation, currentIndentation);
    }
    if (this->currentLine.find_first_of(':') == std::string::npos && !isArrayElment(this->currentLine)) {
        throw IllegalArgumentException("Config sdffile is not valid");
    }
    indentation = this->caluclateIndenetation();
    if (currentIndentation < indentation)
    {
        root = this->getNextToken();
        root->getData()->indentation = indentation;
        this->tokenizeConfigFiles(root, currentIndentation, indentation);
        if (currentNode == nullptr) {
            currentNode = root;
            this->ast.addRoote(root);
        }
        else
         currentNode->addChild(root);
        return;
    }
    if (currentIndentation == indentation) {
        root = this->getNextToken();
        root->getData()->indentation = indentation;
        if (currentNode != nullptr &&currentNode->getData() != nullptr)
            if (currentNode->getParent() != nullptr)
                currentNode = currentNode->getParent();
        if (currentNode == nullptr) {
            currentNode = root;
            this->ast.addRoote(root);
        }
        else
            currentNode->addChild(root);
        this->tokenizeConfigFiles(currentNode, lastIndentation, lastIndentation);
        return ;
    }
    return this->tokenizeConfigFiles(currentNode, currentIndentation, indentation);
}

/**
 *  @Algorithm:
 *      1- calculate the indenetation of the current line
 *
 * */
int ConfigParser::caluclateIndenetation() {
    int indentation = 0;
    for (int i = 0; i < this->currentLine.size(); i++) {
        if (this->currentLine[i] == ' ' || this->currentLine[i] == '\t') {
            indentation++;
        } else {
            break;
        }
    }
    return indentation;
}

Node<Token *> *ConfigParser::getNextToken() {
        Node<Token *> *node = new Node<Token *>();
        node->setData(new Token(this->currentLine.substr(0, this->currentLine.find_first_of(':')), KEYWORD));
        this->currentLine = this->currentLine.substr(this->currentLine.find_first_of(':') + 1);
        if (isEmptyLine(this->currentLine) || isOnlyComment(this->currentLine)) {
            return node;
        }
        if (this->currentLine.find_first_of('#') != std::string::npos) {
            this->currentLine = this->currentLine.substr(0, this->currentLine.find_first_of('#'));
        }
        node->addChild(new Node<Token *>(new Token(trim(this->currentLine), VALUE)));
        return node;
}



int main()
{
    ConfigParser *configParser = new ConfigParser("./config/default.yml");
    try {
    configParser->tokenizeConfigFiles(nullptr, -1, -1);

        Node<Token *> *root = configParser->ast.get(0);
        if (root != nullptr)
            root->printNode(root);
    }catch (IllegalArgumentException &e) {
        std::cout <<"sdfsd"  << e.what() << std::endl;
    }
    //system("leaks webserv");
    return 0;
}