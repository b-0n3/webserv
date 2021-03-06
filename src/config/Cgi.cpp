
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/wait.h>
#include <fcntl.h>
#include "Cgi.h"
#include "../network/StatusCode.h"


std::string getParams(std::map<std::string, std::string> params);

Cgi::Cgi(std::string path, std::vector<std::string> ext) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        throw FileNotFoundException("cgi File not found");
    }
    close(fd);
    if (ext.size() == 0)
        throw IllegalArgumentException("extension not found");
    this->binaryPath = path;
    this->ext = ext;
}

Cgi::~Cgi() {

}

bool Cgi::isCgi(std::string const path) const {
    if (path.find('.') == std::string::npos)
        return false;
    std::string e = path.substr(path.find_last_of('.'));
    if (std::find(this->ext.begin(), this->ext.end(), e) != this->ext.end())
        return true;
    return false;
}

std::string Cgi::getBinaryPath() {
    return this->binaryPath;
}


std::vector<std::string> Cgi::getExt() {
    return this->ext;
}

Cgi *Cgi::fromNode(Node<Token *> *root) {
    std::string path = "";
    std::vector<std::string> ext;
    if (root->getChildren().size() != 2)
        throw IllegalArgumentException("cgi node must have 2 children");
    for (unsigned long i = 0; i < root->getChildren().size(); i++) {
        if (root->getChildren()[i]->getData()->getValue() == "path") {
            if (root->getChildren()[i]->getChildren().size() != 1)
                throw IllegalArgumentException("path node must have 1 child");
            path = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        } else if (root->getChildren()[i]->getData()->getValue() == "ext") {
            if (root->getChildren()[i]->getChildren().empty())
                throw IllegalArgumentException("ext node must have at least 1 child");
            for (unsigned long j = 0; j < root->getChildren()[i]->getChildren().size(); j++) {
                ext.push_back(root->getChildren()[i]->getChildren()[j]->getData()->getValue());
            }
        } else
            throw IllegalArgumentException("cgi node must have path and ext children");
    }
    if (path.empty())
        throw IllegalArgumentException("cgi node must have path child");
    if (ext.empty())
        throw IllegalArgumentException("cgi node must have ext child");
    return new Cgi(path, ext);
}


void headerToEnv(std::string key, std::string value) {
    std::string env = "HTTP_";
    for (unsigned long i = 0; i < key.size(); i++) {
        if (key[i] == '-')
            env += '_';
        else
            env += std::toupper(key[i]);
    }
    setenv(env.c_str(), value.c_str(), 1);
}


void createEnv(HttpRequest *request) {

    std::string script_name = request->GetPath();


    std::string script_path = request->root +
                              request->GetPath();
    std::string query_string = getParams(request->GetParams());

    setenv("DOCUMENT_URI", request->getRealPath().c_str(), 1);                 /* =/cgi-bin/php/hello.php */
    setenv("REQUEST_URI", (request->getRealPath() + "?" + query_string).c_str(),
           1);                                            /* =/cgi-bin/php/hello.php */
    setenv("SCRIPT_NAME", "", 1);/* =/cgi-bin/php/hello.php */
    setenv("PATH_INFO" , request->getRealPath().c_str(),1);
    setenv("SCRIPT_FILENAME", script_path.c_str(),
           1);                                         /* =/APP/examples/cgi-bin/php/hello.php */
    setenv("PATH_TRANSLATED", request->getRealPath().substr(0,
                                                            request->getRealPath().find_last_of('/')).c_str(),
           1);                                         /* =/APP/examples/cgi-bin/php/hello.php */
    setenv("QUERY_STRING", query_string.c_str(), 1);                                            /* = */
    setenv("SERVER_NAME", request->GetHeadersValueOfKey("Host").c_str(),
           1 /*+ request.getHeaders(response.get_server()[response.getServerIndex()].get_server_names()[response.getServerIndex()])*/);                                             /* =localhost */
    setenv("SERVER_PORT", std::to_string(request->port).c_str(),
           1);                                             /* =82 */
    setenv("REQUEST_METHOD", request->GetMethod().c_str(), 1);                                          /* =GET */
    setenv("DOCUMENT_ROOT", request->root.c_str(), 1);                                           /* =/APP/examples */
    setenv("GETAWAY_INTERFACE", "CGI/1.1", 1);                                       /* =CGI/1.1 */
    setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);                                         /* =HTTP/1.1 */
    setenv("REDIRECT_STATUS", "200", 1);                                         /* =200 */
    setenv("FCGI_ROLE", "RESPONDER", 1);                                               /* =RESPONDER */
    setenv("REQUEST_SCHEME", "http", 1);                                          /* =http */
    setenv("SERVER_SOFTWARE", "webserv/1.1", 1);                                         /* =webserv/1.1 (Linux) */

    setenv("REMOTE_ADDR", "0.0.0.0",
           1/*+ request->remoteAddress*/);                                             /* =0.0.0.0 */
    setenv("REMOTE_PORT", "0",
           1  /**+ std::to_string(request->port)*/);                                             /* =0 */
    for (HeadersMap::iterator it = request->GetHeaders().begin();
         it != request->GetHeaders().end();
         ++it) {
        headerToEnv((*it).first, (*it).second);
    }
    if (request->IsHasBody()) {
        setenv("CONTENT_TYPE", request->GetHeadersValueOfKey("Content-Type").c_str(), 1);
        setenv("CONTENT_LENGTH", request->GetHeadersValueOfKey("Content-Length").c_str(), 1);
    }

}

std::string getParams(std::map<std::string, std::string> params) {
    std::stringstream ss;
    for (std::map<std::string, std::string>::iterator it = params.begin(); it != params.end(); it++)
        ss << it->first << "=" << it->second << "&";
    return ss.str();
}


void Cgi::execute(HttpRequest *request, HttpResponse *response) {
    if (!request->cgiRunning) {
        std::string path = request->root + request->GetPath();

        const char *args[3];
        args[0] = this->binaryPath.c_str();
        args[1] = path.c_str();
        args[2] = nullptr;

        pid_t pid = fork();
        if (pid == -1) {
            response->setStatusCode(INTERNAL_SERVER_ERROR);
            return;
        }

        if (pid == 0) {

            createEnv(request);
            close(STDERR_FILENO);
            if (request->IsHasBody()) {
                dup2(request->GetBodyFd(), STDIN_FILENO);
                dup2(response->getBodyFileDescriptor(), STDOUT_FILENO);
                close(request->GetBodyFd());
                close(response->getBodyFileDescriptor());

            } else {

                dup2(response->getBodyFileDescriptor(), STDOUT_FILENO);
                close(response->getBodyFileDescriptor());
                close(0);

            }
            int s = execvp(this->binaryPath.c_str(), (char *const *) args);
            exit(s);
        } else {

            if (request->IsHasBody()) {

                close(request->GetBodyFd());
            }

            request->cgiPid = pid;
            request->cgiRunning = true;

        }
    } else {

        int state;
        int status = waitpid(request->cgiPid, &state, WNOHANG);
        if (status == -1) {
            response->setStatusCode(INTERNAL_SERVER_ERROR);\
            request->cgiRunning = false;
            return;
        } else if (status != 0) {
            if (WIFEXITED(state) == 0) {
                response->setStatusCode(INTERNAL_SERVER_ERROR);

                request->cgiRunning = false;
                return;
            }
            response->getTempFile()._close();
            response->getTempFile()._open();
            response->readFromCgi();
            request->cgiRunning = false;
        }

    }
}
