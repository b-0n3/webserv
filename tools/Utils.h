//
// Created by Abdelouahad Ait hamd on 4/13/22.
//

#ifndef WEBSERV_UTILS_H
#define WEBSERV_UTILS_H

#include <map>
#include <string>
#include <vector>
#include <sys/poll.h>

std::string trim(const std::string &s);

bool is_digits(const std::string &str);

bool is_directory(const std::string &path);

std::string getConentTypeFromFileName(std::string fileName);
struct pollfd *convertToArray(std::vector< struct pollfd>  vec);
std::string autoIndexRead(std::string path);
std::string readFileAndReturnString(std::string filePath);
#endif //WEBSERV_UTILS_H
