//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#include "ConfigParser.h"
#include "../exceptions/FileNotFoundException.h"


ConfigParser::ConfigParser(std::string configFilePath) {
    std::ifstream file(configFilePath);
    if (!file->is_open()) {
        throw FileNotFoundException("Config file not found");
    }
    this->configFile = file;
    this->configFilePath  = configFilePath;
}
bool isEmptyLine(std::string &line)
{
    if (line.find_first_not_of(" \t") == std::string::npos)
        return true;
    return false;
}
bool isOnlyComment(std::string &line)
{
  for(int i = 0; i < line.length(); i++)
  {
      if(line[i] == ' ' || line[i] == '\t')
          continue;
      if (line[i] != '#')
          return false;
  }
  return true;
}


//std::vector<Server> ConfigParser::parseConfigFiles() {
//
////    if (this->configFile.is_open()) {
////        std::string line;
////        if (!std::getline(this->configFile,
////                          this->configFileContent ,
////                          this->configFile.widen(std::ifstream::traits_type::eof())))
////        {
////            this->configFile.close();
////            throw IllegalArgumentException("Config file is empty");
////        }
////    }
//
//    if (this->currentLine.size() <= cursor) {
//        if (  std::getline(this->configFile, this->currentLine).eof())
//            return std::vector<Server>();// @todo:  Change and call other function to convert ast into a vector of server
//    }
//    if (this->currentLine.empty() || isEmptyLine(this->currentLine)) {
//        this->cursor = 0;
//        this->lastCursor = 0;
//        return this->parseConfigFiles();
//    }
//    this->caluclateIndenetation();
//    for (Token *t = this->getNextToken(t); t !=  nullptr;t  =this->getNextToken(t))
//    {
//
//    }
//}
// todo : recuresivly parse the yaml file
std::vector<Server> ConfigParser::parseConfigFiles(Node<Token *> *root, int lastIndentation, int currentIndentation) {
    int indentation;
    Node<Token *> *currentNode = root;
    if (std::getline(this->configFile, this->currentLine).eof())
        return std::vector<Server>();// @todo:  Change and call other function to convert ast into a vector of server;// @todo:  Change and call other function to convert ast into a vector of server
    if ( isEmptyLine(this->currentLine) || isOnlyComment(this->currentLine)) {
        this->cursor = 0;
        this->lastCursor = 0;
        return this->parseConfigFiles(root, lastIndentation, currentIndentation);
    }
    indentation = this->caluclateIndenetation();
    if (currentIndentation < indentation)
    {
        root = this->getNextToken(nullptr);
        this->parseConfigFiles(root, currentIndentation, indentation);
    }
    if (currentIndentation == indentation)
    {
    }
    if (currentIndentation > indentation)
    {
        this->parseConfigFiles(root, currentIndentation, indentation);
    }

    return std::vector<Server>();
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
Node<Token *> *ConfigParser::getNextToken(Token *token) {

}


