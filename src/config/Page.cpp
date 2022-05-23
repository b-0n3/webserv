//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include <fstream>
#include "Page.h"
#include "../exceptions/IllegalArgumentException.h"
#include "../tools/Utils.h"

Page::Page(int errorCode,  std::string contentPath) {
    if (errorCode <= 0 || errorCode >= 599)
        throw IllegalArgumentException("errorCode must be between 100 and 599");
    this->errorCode = errorCode;
    std::ofstream file(contentPath,std::ios::in |std::ios::app);
    if (!file.is_open())
        throw IllegalArgumentException("bad content file :" + contentPath + " File not found");
    file.close();
    this->contentPath = contentPath;
}

int Page::getErrorCode() {
    return this->errorCode;
}

std::string Page::getContentPath() {
    return this->contentPath;
}

Page *Page::fromNode(Node<Token *> *root) {
    int errorCode = -1;
    std::string contentPath ;
    if (root == nullptr)
        throw IllegalArgumentException("root must not be null");
    if (root->getData()->getValue() != "code")
    {
        throw IllegalArgumentException("root must be a code node");
    }
    for (int i = 0; i< root->getChildren().size(); i++)
    {
//           if (root->getChildren()[i]->getChildren().size() != 2)
//               throw IllegalArgumentException("unexpected number of children for code");

               if (is_digits(root->getChildren()[i]->getData()->getValue()))
               {
                   if (errorCode != -1)
                       throw IllegalArgumentException("code must be unique");
                   errorCode = std::stoi(root->getChildren()[i]->getData()->getValue());
               }
               else if (root->getChildren()[i]->getData()->getValue() == "content")
               {
                   contentPath = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
               }
               else
               {
                   throw IllegalArgumentException("unexpected token " + root->getChildren()[i]->getData()->getValue());
               }
    }

    return new Page(errorCode, contentPath);
}
