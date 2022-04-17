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

void ConfigParser::tokenizeConfigFiles(Node<Token *> *parent,Node<Token *> *root, int lastIndentation, int currentIndentation) {
    int indentation;
    Node<Token *> *currentNode = root;
    if (std::getline(*this->configFile, this->currentLine).eof())
        return;// @todo:  Change and call other function to convert ast into a vector of server;// @todo:  Change and call other function to convert ast into a vector of server
    //std::cout << this->currentLine<<std::endl;
    if (isEmptyLine(this->currentLine) || isOnlyComment(this->currentLine)) {

        return this->tokenizeConfigFiles(parent, root, lastIndentation, currentIndentation);
    }
    if (this->currentLine.find_first_of(':') == std::string::npos && !isArrayElment(this->currentLine)) {
        throw IllegalArgumentException("Config sdffile is not valid");
    }
    indentation = this->caluclateIndenetation();
    if (indentation > lastIndentation) {
        root = this->getNextToken();
        root->getData()->indentation = indentation;
        root->setParent(currentNode);

        this->tokenizeConfigFiles(currentNode, root, indentation, indentation);
        if (currentNode == nullptr) {
            currentNode = root;
            this->ast.addRoote(currentNode);
        }else
            currentNode->addChild(root);
        return;
    }
    if (indentation == lastIndentation) {
        root = this->getNextToken();
        root->getData()->indentation = indentation;
        if (currentNode == nullptr) {
            currentNode = root;
            this->ast.addRoote(currentNode);
        }
        else if (currentNode != nullptr && currentNode->getParent() != nullptr) {
            currentNode = currentNode->getParent();
            root->setParent(currentNode);
        }
        this->tokenizeConfigFiles(root, root, indentation, indentation);
        currentNode->addChild(root);
        return;
    }
    if (indentation < lastIndentation)
    {
        if (currentNode == nullptr) {
            currentNode = root;
        }
        else {
            while (indentation <= lastIndentation && currentNode != nullptr && currentNode->getParent() != nullptr) {
                currentNode = currentNode->getParent();
                lastIndentation = currentNode->getData()->indentation;
            }
        }
        root = this->getNextToken();
        root->getData()->indentation = indentation;
        root->setParent(currentNode);
        this->tokenizeConfigFiles(currentNode, root, indentation, indentation);
        currentNode->addChild(root);
        return;
    }
    return this->tokenizeConfigFiles(parent, currentNode, currentIndentation, indentation);
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
        std::string value;
        value = this->currentLine.substr(0, this->currentLine.find_first_of(':'));
        if (value.find_first_of('-') != std::string::npos) {
            value = value.substr(value.find_first_of('-') + 1, value.size());
        }
        node->setData(new Token(trim(value), KEYWORD));
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

std::vector<Server *> ConfigParser::validateAst() {
    std::vector<Server *> servers;
    if (this->ast.getSize() == 0 || this->ast.get(0) == nullptr) {
        throw IllegalArgumentException("Config file is not valid");
    }
    Node<Token *> *root = this->ast.get(0);
    if (root->getData()->getValue() != "servers") {
        throw IllegalArgumentException("Config file is not valid");
    }
    if (root->getChildren().empty())
        throw IllegalArgumentException("no servers found");
    for (int i = 0; i < root->getChildren().size(); i++) {
        Server *s  = Server::fromNode(root->getChildren()[i]);
        servers.push_back(s);
    }

    return servers;
}


int main()
{
    ConfigParser *configParser = new ConfigParser("./config/default.yml");
    try {
    configParser->tokenizeConfigFiles(nullptr, nullptr, -1, -1);

        Node<Token *> *root = configParser->ast.get(0);
     std::vector<Server *> servers =    configParser->validateAst();
//        if (root != nullptr)
//            root->printNode(root);
    }catch (IllegalArgumentException &e) {
        std::cout <<"sdfsd"  << e.   what() << std::endl;
    }
    //system("leaks webserv");
    return 0;
}