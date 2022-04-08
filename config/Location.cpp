//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include "Location.h"

Location::Location(std::string route,
                   std::vector<std::string> allowedMethods,
                   unsigned long maxBodySize,
                   std::string uploadDir,
                   std::vector<std::string> indexFiles,
                   std::vector<Page> errorPages,
                   std::vector<Cgi> cgis) {
    this->setRoute(route);
    this->setAllowedMethods(allowedMethods);
    this->setMaxBodySize(maxBodySize);
    this->setUploadDir(uploadDir);
    this->setIndexFiles(indexFiles);
    this->setErrorPages(errorPages);
    this->setCgis(cgis);
}

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

unsigned long Location::getMaxBodySize() const {
    return this->maxBodySize;
}

void Location::setMaxBodySize(unsigned long maxBodySize) {
    this->maxBodySize = maxBodySize;
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

const std::vector<Page> &Location::getErrorPages() const {
    return this->errorPages;
}

void Location::setErrorPages(const std::vector<Page> &errorPages) {
    this->errorPages = errorPages;
}

const std::vector<Cgi> &Location::getCgis() const {
    return this->cgis;
}

void Location::setCgis(const std::vector<Cgi> &cgis) {
    this->cgis = cgis;
}

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
        for (std::vector<Cgi>::const_iterator it = this->cgis.begin();
             it != this->cgis.end(); ++it) {
            if (it->isCgi(path))
                return (Cgi *) &(*it);
        }
        }
    return nullptr;
}

