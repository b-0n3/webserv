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
//    if(std::find(this->allowedMethods.begin(),
//                 this->allowedMethods.end(),
//                 method) != this->allowedMethods.end()) {
//        return true;
//    }
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
    if(root == nullptr)
        throw IllegalArgumentException("unexpected token");
    l = new Location();
    l->setRoute(root->getData()->getValue());
    for (int i = 0 ;i < root->getChildren().size(); i++)
    {
        if(root->getChildren()[i]->getData()->getValue() == "auto_index")
    }

}

