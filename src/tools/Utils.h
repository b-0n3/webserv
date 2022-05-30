//
// Created by Abdelouahad Ait hamd on 4/13/22.
//

#ifndef WEBSERV_UTILS_H
#define WEBSERV_UTILS_H

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sys/poll.h>
#include <fstream>
std::ifstream::pos_type filesize(const char* filename);

std::string trim(const std::string &s);

bool is_digits(const std::string &str);

bool is_directory(const std::string &path);
size_t countFileSize(const char *filename);
std::string getExtensionByContentType(std::string type);
std::string getContentTypeFromFileName(std::string fileName);
struct pollfd *convertToArray(std::vector< struct pollfd>  vec);
bool autoIndexRead(int fd, std::string path);
std::string readFileAndReturnString(std::string filePath);
std::string toUpper(std::string &str);

struct compareStringIgnoreCase {
    std::string *toUpper( std::string  const &str)const
    {
        std::string *s = new std::string (str);
        for (int i = 0; i < str.size(); i++) {
            (*s)[i] = toupper(str[i]);
        }
        return s;
    }
    bool operator()(const std::string& a, const std::string& b)  const{
        std::string *aLower = toUpper(a);
        std::string *bLower = toUpper(b);
        bool res = *aLower < *bLower;
        delete aLower;
        return res;
    }
};

#endif //WEBSERV_UTILS_H
