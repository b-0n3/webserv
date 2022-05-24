//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include <cstring>
#include <fstream>

#include <fcntl.h>
#include <sys/stat.h>
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

        for (std::vector<Cgi *>::const_iterator it = this->cgis.begin();
             it != this->cgis.end(); ++it) {
            if ((*it)->isCgi(path))
                return *it;
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
                l->addIndexFile(
                        root->getChildren()[i]->getChildren()[j]->getData()->getValue());
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
        if (l->getAllowedMethods().empty())
        {
            l->addAllowedMethod("GET");
            l->addAllowedMethod("POST");
            l->addAllowedMethod("DELETE");
        }
    return l;
}

void Location::setAutoIndex(String autoIndex)  {
    if (autoIndex == "true") {
        this->autoIndex = true;
    } else if (autoIndex == "false") {
        this->autoIndex = false;
    } else {
        throw IllegalArgumentException("unexpected value");
    }
}

void Location::addIndexFile(String indexFile) {
    if (indexFile == "") {
        throw IllegalArgumentException("unexpected value");
    }
    indexFile = indexFile.substr(indexFile.find('-') + 1);
    this->indexFiles.push_back(trim(indexFile));
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

// @todo : write a function that will execute cgi and return the result
void Location::handleCgi(HttpRequest *pRequest, HttpResponse *pResponse) {
    for (int i = 0; i < this->cgis.size(); i++) {
        if (this->cgis[i]->isCgi(pRequest->GetPath())) {

            this->cgis[i]->execute(pRequest, pResponse);

            return;
        }
    }
}

void Location::handleStatic(HttpRequest *pRequest, HttpResponse *pResponse) {
    // check if method is Get
    if (pRequest->GetMethod() == "GET")
        this->handleGet(pRequest, pResponse);
    else if (pRequest->GetMethod() == "POST")
        this->handlePost(pRequest, pResponse);
    else if (pRequest->GetMethod() == "DELETE")
        this->handleDelete(pRequest, pResponse);
    else
        pResponse->setStatusCode(NOT_IMPLEMENTED);
}

const std::string &Location::getRootRir() const {
    return rootRir;
}

void Location::setRootRir(const std::string &rootRir) {
   this->rootRir = rootRir;
}

std::string Location::getIndexFile(String dir) {
    for (int i = 0; i < this->indexFiles.size(); i++) {
        std::string filePath = dir + this->indexFiles[i];
        int fd  = open(filePath.c_str(), O_RDONLY);
        if (fd !=  -1) {
            close(fd);
            return filePath;
        }
    }
    return "";
}

void Location::handleGet(HttpRequest *req, HttpResponse *res) {
    std::string filePath = this->getRootRir() + req->GetPath();
    if  (is_directory(filePath) ) {
        if (filePath [filePath.size() - 1] != '/')
            return this->redirect(req, res, req->GetPath() + "/");
            std::string indexFile = this->getIndexFile(filePath);
            if (!indexFile.empty()) {
                indexFile = indexFile.substr(this->getRootRir().size());
                Cgi *cgi = this->getCgiIfExists(indexFile);
                req->SetPath(indexFile);
                if (cgi != nullptr) {
                    this->handleCgi(req, res);
                    return;
                }
                return this->handleGet(req, res);
            }
        if (this->autoIndex) {

            if (autoIndexRead( res->getBodyFileDescriptor(), filePath)){
                res->setStatusCode(NOT_FOUND);
                return;
            }
            res->setContentType("text/html");
            return;
        }
        return;
    }
   else  if (is_file(filePath)) {
       // std::string content = readFileAndReturnString(filePath);
       int fd;
        if ((fd = open(filePath.c_str(), O_RDONLY )) == 0) {
            res->setStatusCode(NOT_FOUND);
            return;
        }

        res->setContentType(getConentTypeFromFileName(filePath));
        res->setContentLength(countFileSize(filePath.c_str()));
        res->getTempFile().setFd(fd);
       // res->setBody(content);
        return;
    }
   else
        res->setStatusCode(NOT_FOUND);
}

bool Location::is_file(std::string path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode)) {
        return true;
    }
    return false;
}

void Location::redirect(HttpRequest *req, HttpResponse *res, std::string path) {
    res->setStatusCode(FOUND);
    res->addHeader("Location", path);
}

void Location::handlePost(HttpRequest *req, HttpResponse *res) {
    std::string filename = this->uploadDir + req->getBodyFileName() + getExtensionByContentType(req->GetHeadersValueOfKey("Content-Type"));
    std::string filePath =  this->rootRir + "/" +  this->uploadDir + req->getBodyFileName()
            + getExtensionByContentType(req->GetHeadersValueOfKey("Content-Type"));
    if (std::rename(req->getBodyFileName().c_str(), filePath.c_str()) == 0) {


        res->setStatusCode(200);
        res->getTempFile()._close();
        res->getTempFile()._open();
        write(res->getBodyFileDescriptor(), filename.c_str(), filename.length());
        res->getTempFile()._close();
        res->getTempFile()._open();
        res->setContentLength(filename.length());
    } else
        res->setStatusCode(UNAUTHORIZED);

}
void Location::handleDelete(HttpRequest *req, HttpResponse *res) {
    std::string filename = this->rootRir  + "/"  + req->GetPath();
    if (std::strncmp(req->GetPath().c_str(), this->uploadDir.c_str(), this->uploadDir.length()) != 0)
        res->setStatusCode(UNAUTHORIZED);
    else if (unlink(filename.c_str()) > 0)
    {
        res->setStatusCode(OK);
    }
    else
        res->setStatusCode(NOT_FOUND);
}





