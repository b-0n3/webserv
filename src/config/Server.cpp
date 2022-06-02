

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

Server::Server() {
    this->host = "localhost";
    this->port = 80;
    this->root = "/var/www/html";
    this->maxBodySize = 2;
    this->setTimeOut(10);
    this->initParsingMethods();
    this->locations.clear();
    this->errorPages.clear();
    this->redirects.clear();
    this->cgis.clear();
    Redirect *r = new Redirect();
    r->setUrl("");
    r->setStatus(301);
    r->setLocation("/");
    this->redirects.push_back(r);
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

long long Server::getMaxBodySize() const {
    return maxBodySize;
}

void Server::setMaxBodySize(long long maxBodySize) {
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
        for (unsigned long i = 0; i < methods->size(); i++) {
            if (method == methods[i]) {
                this->allowedMethods.push_back(method);
                return;
            }
        }
        throw IllegalArgumentException(method + " : unexpected value");
    } else
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
    if (this->allowedMethods.empty()) {
        allowedMethods.push_back("GET");
        allowedMethods.push_back("POST");
        allowedMethods.push_back("DELETE");
    }
    for (unsigned long i = 0; i < this->locations.size(); i++) {
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
        if (this->locations[i]->getMaxBodySize() == -1)
            this->locations[i]->setMaxBodySize(maxBodySize);
        if (this->locations[i]->getTimeOut() == -1)
            this->locations[i]->setTimeOut(timeOut);
        if (!this->locations[i]->isAutoIndexParsed()) {
            this->locations[i]->setAutoIndex(this->autoIndex);
        }
    }
}


Server *Server::fromNode(Node<Token *> *root) {
    Server *s;
    std::string value;
    if (root->getData()->getValue() != "server")
        throw IllegalArgumentException("unexpected token" + root->getData()->getValue());
    if (root->getChildren().empty())
        throw IllegalArgumentException("empty server not allowed");
    s = new Server();
    try {
        for (unsigned long i = 0; i < root->getChildren().size(); i++) {
            value = root->getChildren()[i]->getData()->getValue();
            if (s->parsingMethods.find(value) != s->parsingMethods.end()) {
                func f = s->parsingMethods[value];
                (s->*f)(root->getChildren()[i]);
            } else
                throw IllegalArgumentException("unexpected token" + root->getChildren()[i]->getData()->getValue());
        }
        if (s->locations.empty())
            throw IllegalArgumentException("no locations defined");
        s->initLocations();
    } catch (std::exception &e) {
        delete s;
        throw e;
    }

    return s;
}


void Server::parseHost(Node<Token *> *n) {
    if (n->getChildren().empty())
        throw IllegalArgumentException("empty host not allowed");
    this->host = n->getChildren()[0]->getData()->getValue();
    if (root.empty())
        throw IllegalArgumentException("no root defined");
}

void Server::parsePort(Node<Token *> *node) {
    std::string value = node->getChildren()[0]->getData()->getValue();

    if (!is_digits(value))
        throw IllegalArgumentException("port must be a number");
    this->port = std::stoi(value);
}

void Server::parseRoot(Node<Token *> *node) {
    if (node->getChildren().empty())
        throw IllegalArgumentException("empty root not allowed");
    this->root = node->getChildren()[0]->getData()->getValue();
    if (root[root.size() - 1] == '/')
        root.pop_back();
}

void Server::parseLocation(Node<Token *> *node) {
    if (node->getChildren().empty())
        throw IllegalArgumentException("empty location not allowed");
    for (unsigned long j = 0; j < node->getChildren().size(); j++) {
        this->locations.push_back(Location::fromNode(node->getChildren()[j]));
    }
}

void Server::parseAutoIndex(Node<Token *> *n) {
    if (n->getChildren().empty())
        throw IllegalArgumentException("empty autoindex not allowed");
    this->setAutoIndex(
            n->getChildren()[0]->getData()->getValue());

}

void Server::parseUploadDir(Node<Token *> *n) {
    if (n->getChildren().empty())
        throw IllegalArgumentException(
                n->getData()->getValue() + " : expected array of values");
    this->setUploadDir(n->getChildren()[0]->getData()->getValue());

}

void Server::parseIndexFiles(Node<Token *> *n) {
    if (n->getChildren().size() == 0)
        throw IllegalArgumentException(
                n->getData()->getValue() + " : expected array of values");
    for (unsigned long j = 0; j < n->getChildren().size(); j++)
        this->addIndexFile(
                n->getChildren()[j]->getData()->getValue());

}

void Server::parseErrorPages(Node<Token *> *n) {
    if (n->getChildren().size() == 0)
        throw IllegalArgumentException(
                n->getData()->getValue() + " : expected array of values");
    for (unsigned long j = 0; j < n->getChildren().size(); j++)
        this->addErrorPage(Page::fromNode(n->getChildren()[j]));

}

void Server::parseCgi(Node<Token *> *node) {
    if (node->getChildren().size() == 0)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected array of values");
    this->addCgi(Cgi::fromNode(node));

}


void Server::parseAllowedMethods(Node<Token *> *node) {
    std::vector<Node<Token *> *> childs = node->getChildren();
    if (!this->getAllowedMethods().empty())
        throw IllegalArgumentException(" redeclaration of allowed methods");
    if (childs.size() == 0)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected array of values");
    for (unsigned long j = 0; j < childs.size(); j++) {
        this->addAllowedMethod(childs[j]->getData()->getValue());
    }
}

void Server::parseMaxBodySize(Node<Token *> *node) {
    if (node->getChildren().size() == 0)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected array of values");
    std::string value = node->getChildren()[0]->getData()->getValue();
    if (!is_digits(value))
        throw IllegalArgumentException("client_max_body_size must be a number");
    this->setMaxBodySize(std::stoi(value));
}

void Server::parseRedirect(Node<Token *> *node) {
    if (node->getChildren().size() == 0)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected array of values");
    for (unsigned long j = 0; j < node->getChildren().size(); j++) {
        this->redirects.push_back(Redirect::fromNode(node->getChildren()[j]));
    }
}


void Server::initParsingMethods() {
    this->parsingMethods["server_name"] = &Server::parseHost;
    this->parsingMethods["listen"] = &Server::parsePort;
    this->parsingMethods["root"] = &Server::parseRoot;
    this->parsingMethods["locations"] = &Server::parseLocation;
    this->parsingMethods["auto_index"] = &Server::parseAutoIndex;
    this->parsingMethods["upload_directory"] = &Server::parseUploadDir;
    this->parsingMethods["index"] = &Server::parseIndexFiles;
    this->parsingMethods["error_page"] = &Server::parseErrorPages;
    this->parsingMethods["cgi"] = &Server::parseCgi;
    this->parsingMethods["allowed_methods"] = &Server::parseAllowedMethods;
    this->parsingMethods["client_max_body_size"] = &Server::parseMaxBodySize;
    this->parsingMethods["redirects"] = &Server::parseRedirect;
    this->parsingMethods["timeout"] = &Server::parseTimeOut;
}

Server::~Server() {
    for (unsigned long i = 0; i < this->locations.size(); i++)
        delete this->locations[i];

    for (unsigned long i = 0; i < this->errorPages.size(); i++)
        delete this->errorPages[i];

    for (unsigned long i = 0; i < this->cgis.size(); i++)
        delete this->cgis[i];
    for (unsigned long i = 0; i < this->redirects.size(); i++)
        delete this->redirects[i];


}

Redirect *Server::getRedirect(std::string path) {
    for (unsigned long i = 0; i < this->redirects.size(); i++) {
        if (this->redirects[i]->getUrl() == path)
            return this->redirects[i];
    }
    return NULL;
}

void Server::parseTimeOut(Node<Token *> *node) {
    if (node->getChildren().size() == 0)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected array of values");
    std::string value =
            node->getChildren()[0]->getData()->getValue();
    if (!is_digits(value))
        throw IllegalArgumentException("timeout must be a number");
    this->setTimeOut(std::stoi(value));
}

long Server::getTimeOut() const {
    return timeOut;
}

void Server::setTimeOut(long timeOut) {
    Server::timeOut = timeOut;
}

long long Server::getMaxBodySize(HttpRequest *request) {
    Location *l = this->getLocation(request->GetPath());
    if (l != NULL)
        return l->getMaxBodySize() * 1e6;
    return this->maxBodySize * 1e6;
}

long Server::getTimeOutAt(HttpRequest *request) {
    Location *l = this->getLocation(request->GetPath());
    if (l != NULL)
        return l->getTimeOut();
    return 60;
}

