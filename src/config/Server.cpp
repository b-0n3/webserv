//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include "Server.h"



Server::Server(std::string host, int port, std::string root) {
    this->host = host;
    this->port = port;
    this->root = root;

}

std::string Server::getHost() {
    return this->host;
}

int Server::getPort() {
    return this->port;
}

std::string Server::getRoot() {
    return this->root;
}

std::vector<Location *> Server::getLocations() {
    return this->locations;
}


bool Server::requestBelongToThisServer(std::string host, int port) {
    return this->host == host && this->port == port;
}

Location *Server::getLocation(std::string path) {
    for (std::vector<Location *>::iterator it = this->locations.begin();
    it != this->locations.end();
    ++it) {
        if ((*it)->isInLocation(path)) {
            return *it;
        }
    }
    return nullptr;
}

Server *Server::fromNode(Node<Token *> *root) {
    Server *s;
    std::string value;
    if (root->getData()->getValue() != "server")
        throw IllegalArgumentException("unexpected token" + root->getData()->getValue());
    if (root->getChildren().empty())
        throw IllegalArgumentException("empty server not allowed");
    s = new Server();
    for(int i = 0; i < root->getChildren().size(); i++)
    {
        value = root->getChildren()[i]->getData()->getValue();
        if (value == "server_name") {
            s->host = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        }
        else if (value == "listen") {
            if (!is_digits(root->getChildren()[i]->getChildren()[0]->getData()->getValue()))
                throw IllegalArgumentException("port must be a number");
            s->port = std::stoi(root->getChildren()[i]->getChildren()[0]->getData()->getValue());
        }
        else if (value == "root") {
            s->root = root->getChildren()[i]->getChildren()[0]->getData()->getValue();

        }
        else if (value == "locations") {
            for (int j = 0; j < root->getChildren()[i]->getChildren().size();j ++) {
                s->locations.push_back(Location::fromNode(root->getChildren()[i]->getChildren()[j]));
            }
        }
        else if (value == "client_max_body_size") {
            if (!is_digits(root->getChildren()[i]->getChildren()[0]->getData()->getValue()))
                throw IllegalArgumentException("client_max_body_size must be a number");
            s->setMaxBodySize(  std::stoi(root->getChildren()[i]->getChildren()[0]->getData()->getValue()));
        }
        else if (value == "allowed_methods") {
            if (!s->getAllowedMethods().empty())
                throw IllegalArgumentException(" redecalaration of allowed methods");
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++) {
                s->addAllowedMethod(root->getChildren()[i]->getChildren()[j]->getData()->getValue());
            }
        } else if (value == "cgi") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            s->addCgi(Cgi::fromNode(root->getChildren()[i]));
        } else if (value == "error_page") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++)
                s->addErrorPage(Page::fromNode(root->getChildren()[i]->getChildren()[j]));
        } else if (value == "upload_directory") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            s->setUploadDir(root->getChildren()[i]->getChildren()[0]->getData()->getValue());
        }else if (value == "index") {
            if (root->getChildren()[i]->getChildren().size() == 0)
                throw IllegalArgumentException(
                        root->getChildren()[i]->getData()->getValue() + " : expected array of values");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++)
                s->addIndexFile(
                        root->getChildren()[i]->getChildren()[j]->getData()->getValue());
        }
        else
            throw IllegalArgumentException("unexpected token" + root->getChildren()[i]->getData()->getValue());
    }
    if (s->locations.size() == 0)
        throw IllegalArgumentException("no locations defined");
    s->initLocations();
    return s;
}

Server::Server() {
    this->host = "localhost";
    this->port = 80;
    this->root = "/var/www/html";
    this->maxBodySize = 1024;
}

void Server::setHost(const std::string &host) {
    Server::host = host;
}

void Server::setPort(int port) {
    Server::port = port;
}

void Server::setRoot(const std::string &root) {
    Server::root = root;
}

void Server::setLocations(const std::vector<Location *> &locations) {
    Server::locations = locations;
}

bool Server::isAutoIndex() const {
    return autoIndex;
}

void Server::setAutoIndex(std::string autoIndex) {
    if (autoIndex == "true") {
        this->autoIndex = true;
    } else if (autoIndex == "false") {
        this->autoIndex = false;
    } else {
        throw IllegalArgumentException("unexpected value");
    }
}

const std::string &Server::getUploadDir() const {
    return uploadDir;
}

void Server::setUploadDir(const std::string &uploadDir) {
    Server::uploadDir = uploadDir;
}

const std::vector<std::string> &Server::getIndexFiles() const {
    return indexFiles;
}

void Server::setIndexFiles(const std::vector<std::string> &indexFiles) {
    Server::indexFiles = indexFiles;
}

const std::vector<Page *> &Server::getErrorPages() const {
    return errorPages;
}

void Server::setErrorPages(const std::vector<Page *> &errorPages) {
    Server::errorPages = errorPages;
}

const std::vector<Cgi *> &Server::getCgis() const {
    return cgis;
}

void Server::setCgis(const std::vector<Cgi *> &cgis) {
    Server::cgis = cgis;
}

const std::string &Server::getRootRir() const {
    return rootRir;
}

void Server::setRootRir(const std::string &rootRir) {
    Server::rootRir = rootRir;
}

unsigned long Server::getMaxBodySize() const {
    return maxBodySize;
}

void Server::setMaxBodySize(unsigned long maxBodySize) {
    Server::maxBodySize = maxBodySize;
}

void Server::addIndexFile(String indexFile) {
    if (indexFiles.empty())
        throw IllegalArgumentException("indexFiles is empty");
    indexFile = indexFile.substr(indexFile.find('-') + 1);
    this->indexFiles.push_back(trim(indexFile));
}

void Server::addAllowedMethod(String method) {
    if (std::count(this->allowedMethods.begin(), this->allowedMethods.end(), method) == 0) {
        String methods[] = {"GET", "POST", "DELETE"};
        this->allowedMethods.push_back(method);
        for (int i = 0; i < methods->size(); i++)
        {
            if (method == methods[i]) {
                this->allowedMethods.push_back(method);
                return ;
            }
        }
        throw IllegalArgumentException(method + " : unexpected value");
    }else
        throw IllegalArgumentException("method already exists");
}

void Server::addCgi(Cgi *cgi) {
    if (cgi != nullptr)
        cgis.push_back(cgi);
}

void Server::addErrorPage(Page *page) {
    if (page != nullptr)
        errorPages.push_back(page);
}

const std::vector<std::string> &Server::getAllowedMethods() const {
    return allowedMethods;
}

void Server::setAllowedMethods(const std::vector<std::string> &allowedMethods) {
    Server::allowedMethods = allowedMethods;
}

void Server::initLocations() {
    if (this->allowedMethods.empty())
    {
        allowedMethods.push_back("GET");
        allowedMethods.push_back("POST");
        allowedMethods.push_back("DELETE");
    }
    for (int i = 0; i < this->locations.size();i++) {
        if (this->locations[i]->getAllowedMethods().empty())
            this->locations[i]->setAllowedMethods(allowedMethods);
        if (this->locations[i]->getCgis().empty())
            this->locations[i]->setCgis(cgis);
        if (this->locations[i]->getErrorPages().empty())
            this->locations[i]->setErrorPages(errorPages);
        if (this->locations[i]->getIndexFiles().empty())
            this->locations[i]->setIndexFiles(indexFiles);
        if (this->locations[i]->getRootRir().empty())
            this->locations[i]->setRootRir(this->root);
//        if (this->locations[i]->isAutoIndexParsed() == false)
//            this->locations[i]->setAutoIndex(autoIndex);
    }
}
