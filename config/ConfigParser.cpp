//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#include "ConfigParser.h"
#include "../exceptions/FileNotFoundException.h"


void ConfigParser::addConfigFile(std::string file) {

}

ConfigParser::ConfigParser(std::string configFilePath) {
    std::ifstream *file = new std::ifstream (configFilePath);
    if (!file->is_open()) {
        throw FileNotFoundException("Config file not found");
    }
    this->configFile = file;
    this->configFilePath  = configFilePath;
}

std::vector<Server> ConfigParser::parseConfigFiles() {

    if (this->configFile->is_open()) {
        std::string line;
        while (getline(*this->configFile, line)) {
            std::vector<std::string> tokens = this->tokenize(line);
            if (tokens.size() == 3) {
                Server server = Server(tokens[0], tokens[1], tokens[2]);
                this->servers.push_back(server);
            }
        }
    }
    this->cursor++;
    this->parseConfigFiles();
}
