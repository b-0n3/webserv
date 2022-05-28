//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_PAGE_H
#define WEBSERV_PAGE_H
#include <string>
#include "../container/Node.h"
#include "Token.h"
#include <fcntl.h>

class Page {
private:
    int minErorrCode;
    int maxErrorCode;
    std::string contentPath;
public:
    Page(std::string errorCode, std::string contentPath);
    std::string getContentPath();
    static Page *fromNode(Node<Token *> *root);
    Page *isInThisPage(int errorCode);

    int openFile();
};


#endif //WEBSERV_PAGE_H
