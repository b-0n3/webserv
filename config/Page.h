//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_PAGE_H
#define WEBSERV_PAGE_H
#include <string>
#include "../container/Node.h"
#include "Token.h"

class Page {
private:
    int errorCode;
    std::string contentPath;
public:
    Page(int errorCode, std::string contentPath);
    int getErrorCode();
    std::string getContentPath();

    static Page *fromNode(Node<Token *> *root);
};


#endif //WEBSERV_PAGE_H
