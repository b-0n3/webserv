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

std::vector<Server> ConfigParser::parseConfigFiles() {

//    if (this->configFile.is_open()) {
//        std::string line;
//        if (!std::getline(this->configFile,
//                          this->configFileContent ,
//                          this->configFile.widen(std::ifstream::traits_type::eof())))
//        {
//            this->configFile.close();
//            throw IllegalArgumentException("Config file is empty");
//        }
//    }

    if (this->currentLine.size() <= cursor) {
        if (  std::getline(this->configFile, this->currentLine).eof())
            return std::vector<Server>();// @todo:  Change and call other function to convert ast into a vector of server
    }
    if (this->indentationStack.empty())
        this->indentationStack.push(0);
    Token *token;
        // todo : get tokens and parse them
       while(( token = this->getNextToken(nullptr)) != nullptr) {
            if (token->getType() == UNKNOWN){
                delete token;
                continue;
            }
            if (token->getType() == BEFOR_CECK) {

            }
        // @Todo: check if the ast root is null and then make token the root or else add it to the ast a  indentation  level goes
    }
       this->indentationStack.push(this->currentIndentation);
       this->lastIndentation = this->currentIndentation;
       this->currentIndentation = 0;
       return this->parseConfigFiles();
}

Token *ConfigParser::getNextToken(Token *token) {
    if (token == nullptr) {
        token = new Token("", UNKNOWN);
    }

    if (this->currentLine.find('#') != std::string::npos) {
        this->currentLine = this->currentLine.substr(0, this->currentLine.find('#'));
        return this->getNextToken(token);
    }
    if (this->currentLine.size() <= this->cursor) {
        if (this->cursor > this->lastCursor){
            token->setType(BEFOR_CECK);
            token->setValue(this->currentLine.substr(this->lastCursor, this->cursor - this->lastCursor));
        }
        this->cursor = 0;
        this->lastCursor = 0;
        return token;
    }

    if (this->currentLine.empty() ||std::isblank(this->currentLine[this->cursor])) {
        return token;
    }
    if (this->currentLine[this->cursor] == ':') {
        token->setType(BEFOR_CECK);
        token->setValue(this->currentLine.substr(this->lastCursor, this->cursor - this->lastCursor));
        this->cursor++
        return token;
    }
//  // @ToDO:  calculate the indentation level and push if def

    if (std::isblank(this->currentLine[this->cursor])) {
        this->currentIndentation++;
        this->cursor++;
    }
    return this->getNextToken(token);
}
