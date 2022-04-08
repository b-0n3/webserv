//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#ifndef WEBSERV_CONFIGPARSER_H
#define WEBSERV_CONFIGPARSER_H
#include "Server.h"
#include <vector>
#include <string>
#include <fstream>
class ConfigParser {
private:
    std::string configFilePath;
    int cursor;
    std::ifstream *configFile;
    int lastCursor;
    int curlyBracketCount;
public:
    ConfigParser(std::string configDir);
    void addConfigFile(std::string file);
    std::vector<Server> parseConfigFiles();
};


#endif //WEBSERV_CONFIGPARSER_H
