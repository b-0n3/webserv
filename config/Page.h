//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_PAGE_H
#define WEBSERV_PAGE_H
#include <string>

class Page {
private:
    int errorCode;
    std::string errorMessage;
    std::string contentPath;
public:
    Page(int errorCode, std::string errorMessage, std::string contentPath);
    int getErrorCode();
    std::string getErrorMessage();
    std::string getContentPath();
};


#endif //WEBSERV_PAGE_H
