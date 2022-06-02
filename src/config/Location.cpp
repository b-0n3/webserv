

#include <cstring>
#include <fstream>

#include <fcntl.h>
#include <sys/stat.h>
#include "Location.h"


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

bool Location::isAllowedMethod(const std::string &method) const {
    if (std::find(this->allowedMethods.begin(),
                  this->allowedMethods.end(),
                  method) != this->allowedMethods.end()) {
        return true;
    }
    return false;
}

bool Location::isInLocation(const std::string &path) const {
    if (path.find(this->route) != std::string::npos) {
        return true;
    }
    return false;
}


Cgi *Location::getCgiIfExists(const std::string &path) const {

    for (std::vector<Cgi *>::const_iterator it = this->cgis.begin();
         it != this->cgis.end(); ++it) {
        if ((*it)->isCgi(path))
            return *it;
    }

    return nullptr;
}


void Location::setAutoIndex(String autoIndex) {
    if (autoIndex == "true") {
        this->autoIndex = true;
    } else if (autoIndex == "false") {
        this->autoIndex = false;
    } else {
        throw IllegalArgumentException("unexpected value");
    }
    autoIndexParsed = true;
}

void Location::addIndexFile(String indexFile) {
    if (indexFile.empty()) {
        throw IllegalArgumentException("unexpected value");
    }
    indexFile = indexFile.substr(indexFile.find('-') + 1);
    this->indexFiles.push_back(trim(indexFile));
}

Location::Location() {
    this->route = "";
    this->autoIndex = false;
    this->indexFiles = std::vector<std::string>();
    this->allowedMethods = std::vector<std::string>();
    this->stripPrefix = false;
    this->timeOut = -1;
    this->maxBodySize = -1;
    initParsingMethods();

}

void Location::addAllowedMethod(String method) {
    String methods[] = {"GET", "POST", "DELETE"};
    if (std::find(this->allowedMethods.begin(), this->allowedMethods.end(), method) != this->allowedMethods.end())
        throw IllegalArgumentException(method + "double declaration  of an  allowed method");
    for (unsigned long i = 0; i < methods->size(); i++) {
        if (method == methods[i]) {
            this->allowedMethods.push_back(method);
            return;
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

void Location::handleCgi(HttpRequest *pRequest, HttpResponse *pResponse) {
    std::string realPath = pRequest->GetPath();
    for (unsigned long i = 0; i < this->cgis.size(); i++) {
        if (this->cgis[i]->isCgi(pRequest->GetPath())) {
            if (this->stripPrefix && pRequest->GetPath() == pRequest->getRealPath()) {
                pRequest->SetPath(pRequest->GetPath().substr(this->route.length()));
            }
            this->cgis[i]->execute(pRequest, pResponse);
            pRequest->SetPath(realPath);
            return;
        }
    }
}

void Location::handleStatic(HttpRequest *pRequest, HttpResponse *pResponse) {
    std::string realPath = pRequest->GetPath();
    if (this->stripPrefix)
        pRequest->SetPath(pRequest->GetPath().substr(this->route.length()));
    if (pRequest->GetMethod() == "GET")
        this->handleGet(pRequest, pResponse);
    else if (pRequest->GetMethod() == "POST")
        this->handlePost(pRequest, pResponse);
    else if (pRequest->GetMethod() == "DELETE")
        this->handleDelete(pRequest, pResponse);
    else
        pResponse->setStatusCode(NOT_IMPLEMENTED);
    pRequest->SetPath(realPath);
}

const std::string &Location::getRootRir() const {
    return rootRir;
}

void Location::setRootRir(const std::string &rootRir) {
    this->rootRir = rootRir;
}

std::string Location::getIndexFile(String dir) {
    for (unsigned long i = 0; i < this->indexFiles.size(); i++) {
        std::string filePath = dir + this->indexFiles[i];
        int fd = open(filePath.c_str(), O_RDONLY);
        if (fd != -1) {
            close(fd);
            return filePath;
        }
    }
    return "";
}

void Location::handleGet(HttpRequest *req, HttpResponse *res) {
    std::string filePath = this->getRootRir() + req->GetPath();
    if (is_directory(filePath)) {
        if (filePath[filePath.size() - 1] != '/')
            return this->redirect(res, req->getRealPath() + "/");
        std::string indexFile = this->getIndexFile(filePath);
        if (!indexFile.empty()) {
            indexFile = indexFile.substr(this->getRootRir().size());
            Cgi *cgi = this->getCgiIfExists(indexFile);

            req->setRealPath(indexFile);
            req->SetPath(indexFile);
            if (cgi != nullptr) {
                if (this->stripPrefix)
                    indexFile = this->route + indexFile;
                req->SetPath(indexFile);
                req->setRealPath(indexFile);
                this->handleCgi(req, res);
                return;
            }
            return this->handleGet(req, res);
        }
        if (this->autoIndex) {

            if (!autoIndexRead(res->getBodyFileDescriptor(), filePath)) {
                res->setStatusCode(NOT_FOUND);
                return;
            }
            res->setContentLength(countFileSize(res->getTempFile().getFileName().c_str()));
            res->setStatusCode(OK);
            res->getTempFile()._close();
            res->getTempFile()._open();
            res->setContentType("text/html");
            return;
        }
        return;
    } else if (is_file(filePath)) {

        int fd;
        if ((fd = open(filePath.c_str(), O_RDONLY)) == 0) {
            res->setStatusCode(NOT_FOUND);
            return;
        }

        res->setContentType(getContentTypeFromFileName(filePath));
        res->setContentLength(countFileSize(filePath.c_str()));
        res->getTempFile().setFd(fd);

        return;
    } else
        res->setStatusCode(NOT_FOUND);
}

bool Location::is_file(std::string path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode)) {
        return true;
    }
    return false;
}


void Location::redirect(HttpResponse *res, std::string path) {
    res->setStatusCode(FOUND);

    res->addHeader("Location", path);
}

void Location::handlePost(HttpRequest *req, HttpResponse *res) {
    std::string filePath = this->getRootRir() + req->GetPath();
    if (is_directory(filePath)) {
        if (filePath[filePath.size() - 1] != '/')
            filePath += "/";
        std::string indexFile = this->getIndexFile(filePath);
        if (!indexFile.empty()) {
            indexFile = indexFile.substr(this->getRootRir().size());
            Cgi *cgi = this->getCgiIfExists(indexFile);

            req->setRealPath(indexFile);
            req->SetPath(indexFile);
            if (cgi != nullptr) {
                if (this->stripPrefix)
                    indexFile = this->route + indexFile;
                req->SetPath(indexFile);
                req->setRealPath(indexFile);
                this->handleCgi(req, res);
                return;
            }
        }
    }
    std::string bodyFilename = req->getBodyFileName().substr(req->getBodyFileName().find_last_of("/") + 1);
    std::string filename = uploadDir + "/"+ bodyFilename + getExtensionByContentType(req->GetHeadersValueOfKey("Content-Type"));
    filePath = this->getUploadDir() + "/" + bodyFilename
               + getExtensionByContentType(req->GetHeadersValueOfKey("Content-Type"));
    if (std::rename(req->getBodyFileName().c_str(), filePath.c_str()) == 0) {

        res->setStatusCode(200);
        res->getTempFile()._close();
        res->getTempFile()._open();
        write(res->getBodyFileDescriptor(), filename.c_str(), filename.length());
        res->getTempFile()._close();
        res->getTempFile()._open();
        res->setContentLength(filename.length());
    } else {
        res->setStatusCode(UNAUTHORIZED);
    }

}

void Location::handleDelete(HttpRequest *req, HttpResponse *res) {

    std::string filePath = this->getRootRir() + req->GetPath();
    if (is_directory(filePath)) {
        if (filePath[filePath.size() - 1] != '/')
            filePath += "/";
        std::string indexFile = this->getIndexFile(filePath);
        if (!indexFile.empty()) {
            indexFile = indexFile.substr(this->getRootRir().size());
            Cgi *cgi = this->getCgiIfExists(indexFile);

            req->setRealPath(indexFile);
            req->SetPath(indexFile);
            if (cgi != nullptr) {
                if (this->stripPrefix)
                    indexFile = this->route + indexFile;
                req->SetPath(indexFile);
                req->setRealPath(indexFile);
                this->handleCgi(req, res);
                return;
            }
        }
    }

    if (uploadDir.empty()) {
        res->setStatusCode(NOT_FOUND);
        return;
    }
    std::string filename = this->uploadDir + "/" + req->GetPath();

    if (unlink(filename.c_str()) == 0) {
        res->setStatusCode(OK);
    } else
        res->setStatusCode(UNAUTHORIZED);
}

const std::string &Location::getRoute() const {
    return route;
}

bool Location::isAutoIndex() const {
    return autoIndex;
}

Location *Location::fromNode(Node<Token *> *root) {
    Location *l;
    if (root == nullptr)
        throw IllegalArgumentException("unexpected token");
    l = new Location();
    try {
        l->setRoute(root->getData()->getValue());
        for (unsigned long i = 0; i < root->getChildren().size(); i++) {
            String value = root->getChildren()[i]->getData()->getValue();
            if (l->parsingMethods.find(value) != l->parsingMethods.end()) {
                func f = l->parsingMethods[value];
                (l->*f)(root->getChildren()[i]);
            } else
                throw IllegalArgumentException(root->getChildren()[i]->getData()->getValue() + " : unexpected token");
        }
    }
    catch (std::exception &e) {
        delete l;
        throw e;
    }

    return l;
}

void Location::parseRoot(Node<Token *> *n) {
    if (n->getChildren().size() != 1)
        throw IllegalArgumentException("root must have  one child");
    this->rootRir = n->getChildren()[0]->getData()->getValue();
    if (rootRir[rootRir.size() - 1] == '/')
        rootRir.pop_back();
}


void Location::parseAutoIndex(Node<Token *> *n) {
    if (n->getChildren().size() != 1)
        throw IllegalArgumentException("autoIndex must have  one child");
    this->setAutoIndex(n->getChildren()[0]->getData()->getValue());

}

void Location::parseUploadDir(Node<Token *> *n) {
    if (n->getChildren().empty())
        throw IllegalArgumentException(
                n->getData()->getValue() + " : expected array of values");
    this->setUploadDir(n->getChildren()[0]->getData()->getValue());

}

void Location::parseIndexFiles(Node<Token *> *n) {
    if (n->getChildren().empty())
        throw IllegalArgumentException(
                n->getData()->getValue() + " : expected array of values");
    for (unsigned long j = 0; j < n->getChildren().size(); j++)
        this->addIndexFile(
                n->getChildren()[j]->getData()->getValue());

}

void Location::parseErrorPages(Node<Token *> *n) {
    if (n->getChildren().empty())
        throw IllegalArgumentException(
                n->getData()->getValue() + " : expected array of values");
    for (unsigned long j = 0; j < n->getChildren().size(); j++)
        this->addErrorPage(Page::fromNode(n->getChildren()[j]));

}


void Location::parseCgi(Node<Token *> *node) {
    if (node->getChildren().empty())
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected array of values");
    this->addCgi(Cgi::fromNode(node));
}

void Location::parseAllowedMethods(Node<Token *> *node) {
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

void Location::parseMaxBodySize(Node<Token *> *node) {
    if (node->getChildren().size() != 1)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected one value");
    std::string value = node->getChildren()[0]->getData()->getValue();
    if (!is_digits(value))
        throw IllegalArgumentException("client_max_body_size must be a number");
    this->setMaxBodySize(std::stoi(value));

}

void Location::parseStripPrefix(Node<Token *> *node) {
    if (node->getChildren().size() != 1)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected one value");
    std::string value = node->getChildren()[0]->getData()->getValue();

    this->setStripPrefix(value);
}

void Location::parseTimeOut(Node<Token *> *node) {
    if (node->getChildren().size() != 1)
        throw IllegalArgumentException(
                node->getData()->getValue() + " : expected one value");
    std::string value = node->getChildren()[0]->getData()->getValue();
    if (!is_digits(value))
        throw IllegalArgumentException("timeout must be a number");
    this->setTimeOut(std::stoi(value));
}

void Location::initParsingMethods() {
    this->parsingMethods["root"] = &Location::parseRoot;
    this->parsingMethods["auto_index"] = &Location::parseAutoIndex;
    this->parsingMethods["upload_directory"] = &Location::parseUploadDir;
    this->parsingMethods["index"] = &Location::parseIndexFiles;
    this->parsingMethods["error_page"] = &Location::parseErrorPages;
    this->parsingMethods["cgi"] = &Location::parseCgi;
    this->parsingMethods["allowed_methods"] = &Location::parseAllowedMethods;
    this->parsingMethods["client_max_body_size"] = &Location::parseMaxBodySize;
    this->parsingMethods["strip_prefix"] = &Location::parseStripPrefix;
    this->parsingMethods["timeout"] = &Location::parseTimeOut;
}

void Location::setAutoIndex1(bool autoIndex) {
    Location::autoIndex = autoIndex;
}

bool Location::isStripPrefix() const {
    return stripPrefix;
}

void Location::setStripPrefix(std::string &stripPrefix) {
    if (stripPrefix == "true") {
        this->stripPrefix = true;
    } else if (stripPrefix == "false") {
        this->stripPrefix = false;
    } else {
        throw IllegalArgumentException("unexpected value");
    }

}

const std::map<std::string, func> &Location::getParsingMethods() const {
    return parsingMethods;
}

void Location::setParsingMethods(const std::map<std::string, func> &parsingMethods) {
    Location::parsingMethods = parsingMethods;
}

long long Location::getMaxBodySize() const {
    return maxBodySize;
}

void Location::setMaxBodySize(long long maxBodySize) {
    Location::maxBodySize = maxBodySize;
}

Location::~Location() {
    for (unsigned long i = 0; i < this->getErrorPages().size(); i++) {
        delete this->getErrorPages()[i];
    }

    for (unsigned long i = 0; i < this->getCgis().size(); i++) {
        delete this->getCgis()[i];
    }

}


bool Location::isAutoIndexParsed() const {
    return autoIndexParsed;
}

void Location::setAutoIndexParsed(bool autoIndexParsed) {
    Location::autoIndexParsed = autoIndexParsed;
}

long Location::getTimeOut() const {
    return timeOut;
}

void Location::setTimeOut(long timeOut) {
    Location::timeOut = timeOut;
}

void Location::setAutoIndex(bool autoIndex) {
    this->autoIndex = autoIndex;
}








