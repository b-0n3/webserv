//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_CGI_H
#define WEBSERV_CGI_H
#include "../exceptions/FileNotFoundException.h"
#include "../exceptions/IllegalArgumentException.h"
#include "../container/Node.h"
#include <vector>
#include "Token.h"
#include <iostream>
#include <algorithm>

class Cgi {
private:
    std::string  binaryPath;
     std::vector<std::string> ext;
public:
    Cgi(std::string  path, std::vector<std::string> ext);
    ~Cgi();
    std::string getBinaryPath();
    std::vector<std::string> getExt();
    bool isCgi(std::string path) const;

    static Cgi *fromNode(Node<Token *> *root);
};


#endif //WEBSERV_CGI_H
