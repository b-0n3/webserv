

#include "ConfigParser.h"
#include "../exceptions/FileNotFoundException.h"


ConfigParser::ConfigParser(std::string configFilePath) {

    std::ifstream *file = new std::ifstream(configFilePath);
    if (!file->is_open()) {
        throw FileNotFoundException("Config file not found");
    }
    this->configFile = file;
    this->configFilePath = configFilePath;
}

bool isEmptyLine(std::string &line) {
    if (line.find_first_not_of(" \t") == std::string::npos)
        return true;
    return false;
}

bool isOnlyComment(std::string &line) {
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t')
        i++;
    if (line[i] == '#')
        return true;
    return false;
}

bool isArrayElment(std::string &line) {
    int i = 0;
    while (line[i] == ' ' || line[i] == '\t')
        i++;
    if (line[i] == '-')
        return true;
    return false;
}

void ConfigParser::tokenizeConfigFiles(Node<Token *> *parent, Node<Token *> *root, int lastIndentation,
                                       int currentIndentation) {
    int indentation;
    Node<Token *> *currentNode = root;

    if (std::getline(*this->configFile, this->currentLine).eof())
        return;
    this->replaceEnv();
    if (isEmptyLine(this->currentLine) || isOnlyComment(this->currentLine)) {

        return this->tokenizeConfigFiles(parent, root, lastIndentation, currentIndentation);
    }
    if (this->currentLine.find_first_of(':') == std::string::npos && !isArrayElment(this->currentLine)) {
        throw IllegalArgumentException("unexpected token" + this->currentLine);
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
        } else
            currentNode->addChild(root);
        return;
    }
    if (indentation == lastIndentation) {
        root = this->getNextToken();
        root->getData()->indentation = indentation;
        if (currentNode == nullptr) {
            currentNode = root;
            this->ast.addRoote(currentNode);
        } else if (currentNode != nullptr && currentNode->getParent() != nullptr) {
            currentNode = currentNode->getParent();
            root->setParent(currentNode);
        }
        this->tokenizeConfigFiles(root, root, indentation, indentation);
        currentNode->addChild(root);
        return;
    }
    if (indentation < lastIndentation) {
        if (currentNode == nullptr) {
            currentNode = root;
        } else {
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


int ConfigParser::caluclateIndenetation() {
    int indentation = 0;
    for (unsigned long i = 0; i < this->currentLine.size(); i++) {
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
    if (trim(this->currentLine).empty())
        return node;
    node->addChild(new Node<Token *>(new Token(trim(this->currentLine), VALUE)));
    return node;
}

std::vector<Server *> ConfigParser::validateAst() {
    std::vector<Server *> servers;
    if (this->ast.getSize() == 0 || this->ast.get(0) == nullptr) {
        throw IllegalArgumentException("Config file  is not valid");
    }
    Node<Token *> *root = this->ast.get(0);
    if (root->getData()->getValue() != "servers") {
        throw IllegalArgumentException("Config file  sda is not valid");
    }
    if (root->getChildren().empty())
        throw IllegalArgumentException("no servers found");
    for (unsigned long i = 0; i < root->getChildren().size(); i++) {
        Server *s;
        try {
            s = Server::fromNode(root->getChildren()[i]);
            servers.push_back(s);
        }
        catch (std::exception &e) {
            servers.push_back(s);
            for (int i = 0; i < servers.size(); i++) {
                delete servers[i];
                servers.erase(servers.begin() + i);
            }
            throw e;
        }
    }

    return servers;
}

BinaryTree<Token *> ConfigParser::getAst() {
    return this->ast;
}

void ConfigParser::replaceEnv() {
    if (std::count(this->currentLine.begin(), this->currentLine.end(), '$') == 0) {
        return;
    }

    std::string lastValue = this->currentLine;
    std::string::size_type start = 0;
    std::string::size_type first = 0;
    std::string::size_type last = 0;
    while (1) {
        if (this->currentLine.find_first_of("${", start) == std::string::npos) {
            break;
        }
        first = this->currentLine.find_first_of('$', start);
        last = this->currentLine.find_first_of('}', first + 1);
        if (last == std::string::npos) {
            break;
        }
        std::string env = this->currentLine.substr(first + 2, last - first - 2);
        start = last + 1;
        char *value = std::getenv(env.c_str());
        if (value == nullptr)
            continue;

        lastValue.replace(first,
                          last - first + 1,
                          value);
    }
    this->currentLine = lastValue;
}

ConfigParser::~ConfigParser() {
    configFile->close();
    delete configFile;
    ast.deleteTree(this->ast.get(0));
}
