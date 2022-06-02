

#ifndef WEBSERV_CGI_H
#define WEBSERV_CGI_H

#include "../exceptions/FileNotFoundException.h"
#include "../exceptions/IllegalArgumentException.h"
#include "../container/Node.h"
#include <vector>
#include "Token.h"
#include <iostream>
#include <algorithm>
#include "../network/HttpRequest.h"
#include "../network/HttpResponse.h"
#include "../tools/Utils.h"

class Cgi {
private:
    std::string binaryPath;
    std::vector<std::string> ext;
public:
    Cgi(std::string path, std::vector<std::string> ext);

    ~Cgi();

    std::string getBinaryPath();

    std::vector<std::string> getExt();

    bool isCgi(std::string path) const;

    static Cgi *fromNode(Node<Token *> *root);

    void execute(HttpRequest *request, HttpResponse *response);
};


#endif //WEBSERV_CGI_H
