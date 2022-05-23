//
// Created by Abdelouahad Ait hamd on 4/13/22.
//

#ifndef WEBSERV_UTILS_H
#define WEBSERV_UTILS_H

#include <map>
#include <string>
#include <vector>
#include <sys/poll.h>
#include <fstream>
std::ifstream::pos_type filesize(const char* filename);

std::string trim(const std::string &s);

bool is_digits(const std::string &str);

bool is_directory(const std::string &path);
size_t countFileSize(const char *filename);

std::string getConentTypeFromFileName(std::string fileName);
struct pollfd *convertToArray(std::vector< struct pollfd>  vec);
bool autoIndexRead(int fd, std::string path);
std::string readFileAndReturnString(std::string filePath);
#endif //WEBSERV_UTILS_H
