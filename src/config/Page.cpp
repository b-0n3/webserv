

#include <fstream>
#include "Page.h"
#include "../exceptions/IllegalArgumentException.h"
#include "../tools/Utils.h"


Page::Page(std::string errorCode, std::string contentPath) {

    if (errorCode.empty()) {
        throw IllegalArgumentException("errorCode is empty");
    }
    if (contentPath.empty()) {
        throw IllegalArgumentException("contentPath is empty");
    }
    if (errorCode.find_first_of('x') != std::string::npos) {
        std::replace(errorCode.begin(), errorCode.end(), 'x', '0');
        this->minErorrCode = std::stoi(errorCode);
        this->maxErrorCode = this->minErorrCode / 100;
        this->maxErrorCode *= 100;
        this->maxErrorCode += 99;
    } else {
        this->minErorrCode = std::stoi(errorCode);
        this->maxErrorCode = this->minErorrCode;
    }
    if (this->minErorrCode < 0 || this->minErorrCode > 999) {
        throw IllegalArgumentException("errorCode is not valid");
    }
    std::ofstream file(contentPath, std::ios::in | std::ios::app);
    if (!file.is_open())
        throw IllegalArgumentException("bad content file :" + contentPath + " File not found");
    file.close();
    this->contentPath = contentPath;
}


std::string Page::getContentPath() {
    return this->contentPath;
}

Page *Page::fromNode(Node<Token *> *root) {
    std::string errorCode = "-1";
    std::string contentPath;
    if (root == nullptr)
        throw IllegalArgumentException("root must not be null");
    if (root->getData()->getValue() != "code") {
        throw IllegalArgumentException("root must be a code node");
    }
    for (size_t i = 0; i < root->getChildren().size(); i++) {

        if (is_digits(root->getChildren()[i]->getData()->getValue())) {
            if (errorCode != "-1")
                throw IllegalArgumentException("duplicated code");
            errorCode = root->getChildren()[i]->getData()->getValue();
        } else if (root->getChildren()[i]->getData()->getValue() == "content") {
            contentPath = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        } else {
            throw IllegalArgumentException("unexpected token " + root->getChildren()[i]->getData()->getValue());
        }
    }

    return new Page(errorCode, contentPath);
}

Page *Page::isInThisPage(int status) {
    if (status >= this->minErorrCode && status <= this->maxErrorCode)
        return this;
    return nullptr;
}

int Page::openFile() {
    return open(this->contentPath.c_str(), O_RDONLY);
}

