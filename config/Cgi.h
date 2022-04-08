//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_CGI_H
#define WEBSERV_CGI_H
#include "../exceptions/FileNotFoundException.h"
#include "../exceptions/IllegalArgumentException.h"
#include <vector>
#include <iostream>


class Cgi {
private:
    std::string const binaryPath;
    const std::vector<std::string> ext;
public:
    Cgi(std::string const path, std::vector<std::string> ext);
    ~Cgi();
    std::string getBinaryPath();
    std::vector<std::string> getExt();
    bool isCgi(std::string path) const;
};


#endif //WEBSERV_CGI_H
