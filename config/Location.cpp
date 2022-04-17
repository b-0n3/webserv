//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include "Location.h"


// @todo: check all given parameters are valid and throw exception if not


std::string Location::getRout() const {
    return this->route;
}

void Location::setRoute(const std::string &rout) {
    this->route = rout;
}

const std::vector<std::string> &Location::getAllowedMethods() const {
    return this->allowedMethods;
}

void Location::setAllowedMethods(const std::vector<std::string> &allowedMethods) {
    this->allowedMethods = allowedMethods;
}



const std::string &Location::getUploadDir() const {
    return this->uploadDir;
}

void Location::setUploadDir(const std::string &uploadDir) {
    this->uploadDir = uploadDir;
}

const std::vector<std::string> &Location::getIndexFiles() const {
    return this->indexFiles;
}

void Location::setIndexFiles(const std::vector<std::string> &indexFiles) {
    this->indexFiles = indexFiles;
}

const std::vector<Page *> &Location::getErrorPages() const {
    return this->errorPages;
}

void Location::setErrorPages(const std::vector<Page *> &errorPages) {
    this->errorPages = errorPages;
}

const std::vector<Cgi *> &Location::getCgis() const {
    return this->cgis;
}

void Location::setCgis(const std::vector<Cgi *> &cgis) {
    this->cgis = cgis;
}
// @todo: verify if the sepecified method is allowed in this location
bool Location::isAllowedMethod(const std::string &method) const {
    if(std::find(this->allowedMethods.begin(),
                 this->allowedMethods.end(),
                 method) != this->allowedMethods.end()) {
        return true;
    }
    return false;
}

bool Location::isInLocation(const std::string &path) const {
    if(path.find(this->route) != std::string::npos) {
        return true;
    }
    return false;
}


Cgi *Location::getCgiIfExists(const std::string &path) const {
    if (this->isInLocation(path)) {
        for (std::vector<Cgi *>::const_iterator it = this->cgis.begin();
             it != this->cgis.end(); ++it) {
            if ((*it)->isCgi(path))
                return *it;
        }
        }
    return nullptr;
}

Location  *Location::fromNode(Node<Token *> *root) {
    Location *l;
    if (root == nullptr)
        throw IllegalArgumentException("unexpected token");
    l = new Location();
    l->setRoute(root->getData()->getValue());
    for (int i = 0; i < root->getChildren().size(); i++) {
        String value = root->getChildren()[i]->getData()->getValue();
        if (value == "auto_index")
            l->setAutoIndex(root->getChildren()[i]->getChildren()[0]->getData()->getValue());
        else if (value == "index") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++)
                l->addIndexFile(root->getChildren()[i]->getChildren()[j]->getData()->getValue());
        } else if (value == "allowed_methods") {
            if (!l->getAllowedMethods().empty())
                throw IllegalArgumentException(" redecalaration of allowed methods");
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++) {
                l->addAllowedMethod(root->getChildren()[i]->getChildren()[j]->getData()->getValue());
            }
        } else if (value == "cgi") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            l->addCgi(Cgi::fromNode(root->getChildren()[i]));
        } else if (value == "error_page") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++)
                l->addErrorPage(Page::fromNode(root->getChildren()[i]->getChildren()[j]));
        } else if (value == "upload_directory") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            l->setUploadDir(root->getChildren()[i]->getChildren()[0]->getData()->getValue());
        } else
            throw IllegalArgumentException(root->getChildren()[i]->getData()->getValue() + " : unexpected token");
    }

    return l;
}

void Location::setAutoIndex(String autoIndex) noexcept(false) {
    if (autoIndex == "true") {
        this->autoIndex = true;
    } else if (autoIndex == "false") {
        this->autoIndex = false;
    } else {
        throw IllegalArgumentException("unexpected value");
    }
}

void Location::addIndexFile(String indexFile) noexcept(false) {
    if (indexFile == "") {
        throw IllegalArgumentException("unexpected value");
    }
    this->indexFiles.push_back(indexFile);
}

Location::Location() {
    this->route = "";
    this->autoIndex = true;
    this->indexFiles = std::vector<std::string>();
    this->allowedMethods = std::vector<std::string>();
}

void Location::addAllowedMethod(String method) {
    String methods[] = {"GET", "POST", "DELETE"};
    if (std::find(this->allowedMethods.begin(), this->allowedMethods.end(), method) != this->allowedMethods.end())
        throw IllegalArgumentException(method + "double declaration  of an  allowed method");
    for (int i = 0; i < methods->size(); i++)
    {
        if (method == methods[i]) {
            this->allowedMethods.push_back(method);
            return ;
        }
    }
    throw IllegalArgumentException(method + " : unexpected value");
}

void Location::addCgi(Cgi *cgi) {
    this->cgis.push_back(cgi);
}

void Location::addErrorPage(Page *page) {
    if (page == nullptr)
        throw IllegalArgumentException("unexpected value");
    this->errorPages.push_back(page);
}



