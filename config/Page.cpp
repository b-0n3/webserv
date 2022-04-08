//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include <fstream>
#include "Page.h"
#include "../exceptions/IllegalArgumentException.h"

Page::Page(int errorCode, std::string errorMessage, std::string contentPath) {
    this->errorCode = errorCode;
    this->errorMessage = errorMessage;
    std::ofstream file(contentPath);
    if (!file.is_open())
        throw IllegalArgumentException("bad content file :" + contentPath + " File not found");
    file.close();
    this->contentPath = contentPath;
}

int Page::getErrorCode() {
    return this->errorCode;
}

std::string Page::getErrorMessage() {
    return this->contentPath;
}

std::string Page::getContentPath() {
    return this->contentPath;
}
