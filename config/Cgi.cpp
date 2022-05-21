//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/wait.h>
#include <fcntl.h>
#include "Cgi.h"
#include "../network/StatusCode.h"


std::string getParams(std::map<std::string, std::string> params);

Cgi::Cgi(std::string  path, std::vector<std::string>  ext) {
int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
      throw FileNotFoundException("cgi File not found");
  }
  close(fd);
   if (ext.size() == 0)
       throw  IllegalArgumentException("extension not found");
    this->binaryPath = path;
    this->ext = ext;
}

Cgi::~Cgi() {

}

 bool Cgi::isCgi(std::string const path) const {
    if (path.find('.') == std::string::npos)
        return false;
    std::string e = path.substr(path.find_last_of('.') );
    if (std::find(this->ext.begin(), this->ext.end(), e) != this->ext.end())
        return true;
    return false;
}

std::string Cgi::getBinaryPath() {
    return this->binaryPath;
}



std::vector <std::string> Cgi::getExt() {
    return this->ext;
}

Cgi *Cgi::fromNode(Node<Token *> *root) {
   std::string path = "";
   std::vector<std::string> ext;
    if (root->getChildren().size() != 2)
        throw IllegalArgumentException("cgi node must have 2 children");
    for (int i = 0; i < root->getChildren().size(); i++)
    {
        if (root->getChildren()[i]->getData()->getValue() == "path") {
            if (root->getChildren()[i]->getChildren().size() != 1)
                throw IllegalArgumentException("path node must have 1 child");
            path = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        }
       else if (root->getChildren()[i]->getData()->getValue() == "ext") {
            if (root->getChildren()[i]->getChildren().empty())
                throw IllegalArgumentException("ext node must have at least 1 child");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++) {
                ext.push_back(root->getChildren()[i]->getChildren()[j]->getData()->getValue());
            }
        }
        else
            throw IllegalArgumentException("cgi node must have path and ext children");
    }
    if (path.empty())
        throw IllegalArgumentException("cgi node must have path child");
    if (ext.empty())
        throw IllegalArgumentException("cgi node must have ext child");
    return new Cgi(path, ext);
}
std::string headerToEnv(std::pair<std::string, std::string> header)
{
    std::string env = "HTTP_";
    for (int i = 0; i < (header).first.size(); i++)
    {
        if ((header).first[i] == '-')
            env += '_';
        else
            env += toupper(header.first[i]);
    }
    env += "=";
    env += header.second;
    return env;
}
 char **createEnv(HttpRequest *request)
{
    std::vector<std::string> envp_vect;
    std::string script_name = request->GetPath();
         //   .substr( request->GetPath().find_last_of('/') );
//    script_name.substr(request->location.size());

    std::string script_path = request->root+
            request->GetPath();
    std::string query_string = getParams(request->GetParams());

    envp_vect.push_back("DOCUMENT_URI=" +request->GetPath());                 /* =/cgi-bin/php/hello.php */
    envp_vect.push_back("REQUEST_URI=" +request->GetPath()+"?" + query_string );                                            /* =/cgi-bin/php/hello.php */
    envp_vect.push_back("SCRIPT_NAME=" +  script_name);/* =/cgi-bin/php/hello.php */
  //  envp_vect.push_back("PATH_INFO=" +request->GetPath().substr(0, request->GetPath().find_last_of('/') + 1));
   envp_vect.push_back("SCRIPT_FILENAME=" +script_path);                                         /* =/APP/examples/cgi-bin/php/hello.php */
    envp_vect.push_back("PATH_TRANSLATED=" +script_path);                                         /* =/APP/examples/cgi-bin/php/hello.php */
    envp_vect.push_back("QUERY_STRING=" + query_string);                                            /* = */
    envp_vect.push_back("SERVER_NAME=" + request->GetHeadersValueOfKey("Host")/*+ request.getHeaders(response.get_server()[response.getServerIndex()].get_server_names()[response.getServerIndex()])*/);                                             /* =localhost */
    envp_vect.push_back("SERVER_PORT=" + std::to_string(request->port));                                             /* =82 */
    envp_vect.push_back("REQUEST_METHOD=" + request->GetMethod());                                          /* =GET */
    envp_vect.push_back("DOCUMENT_ROOT=" + request->root);                                           /* =/APP/examples */
    envp_vect.push_back("GETAWAY_INTERFACE=CGI/1.1");                                       /* =CGI/1.1 */
    envp_vect.push_back("SERVER_PROTOCOL=HTTP/1.1");                                         /* =HTTP/1.1 */
    envp_vect.push_back("REDIRECT_STATUS=200");                                         /* =200 */
    envp_vect.push_back("FCGI_ROLE=RESPONDER");                                               /* =RESPONDER */
    envp_vect.push_back("REQUEST_SCHEME=http");                                          /* =http */
    envp_vect.push_back("SERVER_SOFTWARE=webserv/1.1");                                         /* =webserv/1.1 (Linux) */
    envp_vect.push_back("PATH=" + std::string(std::getenv("PATH")));                                                    /* =/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin */
    envp_vect.push_back("REMOTE_ADDR=0.0.0.0" /*+ request->remoteAddress*/);                                             /* =0.0.0.0 */
    envp_vect.push_back("REMOTE_PORT=0"  /**+ std::to_string(request->port)*/);                                             /* =0 */

    if (request->IsHasBody() )
    {
        envp_vect.push_back("CONTENT_TYPE=" + request->GetHeadersValueOfKey("Content-Type"));
        envp_vect.push_back("CONTENT_LENGTH=" + request->GetHeadersValueOfKey("Content-Length"));
    }
    else
    {
        envp_vect.push_back("CONTENT_TYPE=text/html; charset=UTF-8");
        envp_vect.push_back("CONTENT_LENGTH=0");
    }
   for(std::pair<std::string , std::string > it : request->GetHeaders())
    {
        envp_vect.push_back(headerToEnv(it));
    }

    char **_envp = (char**)malloc(sizeof(char*) * (envp_vect.size() + 1));
    size_t i = 0;
    while (i < envp_vect.size())
    {
        _envp[i] = strdup(envp_vect[i].c_str());
       std::cout << "envp: [" << _envp[i] << "]" << std::endl;
        i++;
    }
    _envp[i] = NULL;
    return _envp;
}

std::string getParams(std::map<std::string, std::string> params) {
    std::stringstream ss;
    for (std::map<std::string, std::string>::iterator it = params.begin(); it != params.end(); it++)
        ss << it->first << "=" << it->second << "&";
    return ss.str();
}

// @todo: write a function that will return a Cgi object from a node
void Cgi::execute(HttpRequest *pRequest, HttpResponse *pResponse) {
    if (!pRequest->cgiRunning) {
        std::string path = pRequest->root + pRequest->GetPath();
        const char *args[3];
        args[0] = this->binaryPath.c_str();
        args[1] = path.c_str();
        args[0] = nullptr;
       // std::cout << "Cgi::execute: " << args[0] << " " << args[1] << std::endl;
        char **env = createEnv(pRequest);
        //int writePipe[2];
       // int readPipe[2];
//        if (pipe(readPipe) == -1) {
//            pResponse->setStatusCode(INTERNAL_SERVER_ERROR);
//            return;
//        }
//        if (!pRequest->IsHasBody()) {
//            if (pipe(writePipe) == -1) {
//                close(readPipe[0]);
//                close(readPipe[1]);
//                pResponse->setStatusCode(INTERNAL_SERVER_ERROR);
//                return;
//            }
//        }
        //int out = dup(STDOUT_FILENO);
        pid_t pid = fork();
        if (pid == -1) {
            pResponse->setStatusCode(INTERNAL_SERVER_ERROR);
            return;
        }
        if (pid == 0) {
         //   std::cout << "execve" << std::endl;
            std::cout << this->binaryPath<< std::endl;
            if (!pRequest->IsHasBody()) {
                dup2( pRequest->GetBodyFd(),  STDIN_FILENO);
                dup2(  pResponse->getBodyFileDescriptor() , STDOUT_FILENO );
                close(pRequest->GetBodyFd());
                close(pResponse->getBodyFileDescriptor());
//               // dup2(out, STDERR_FILENO );
//                close(readPipe[0]);
//                close(writePipe[1]);
//                close(readPipe[1]);
//                close(writePipe[0]);
            } else {
               // close(readPipe[0]);
                dup2( pResponse->getBodyFileDescriptor() , STDOUT_FILENO );
                close(pResponse->getBodyFileDescriptor());
                close(0);
             //  dup2(readPipe[1] , STDERR_FILENO );
              // close(readPipe[1]);
            }
          int s =   execve( this->binaryPath.c_str(), (char *const *) args, env);
            exit(s);
        } else {
         //   dup2(STDERR_FILENO, out);
//            if (!pRequest->IsHasBody()) {
//            //    std::cout << "writing body to cgi" << std::endl;
//                close(writePipe[0]);
////                write(writePipe[1],
////                      pRequest->GetBody().c_str(),
////                      pRequest->GetBody().size());
////                write(writePipe[1],pRequest.
//                close(writePipe[1]);
//            }
          // close(readPipe[1]);
            pRequest->cgiPid = pid;
            pRequest->cgiRunning = true;
         //   pResponse->setCgiReadFd(readPipe[0]);
        }
    }else
    {
      //  std::cout << "Cgi::execute: cgi already running" << std::endl;
        int state;
        int status = waitpid(pRequest->cgiPid, &state, WNOHANG);
        if ( status == -1)
        {
            pResponse->setStatusCode(INTERNAL_SERVER_ERROR);\
            pRequest->cgiRunning = false;
            return;
        }
        else if (status != 0)
        {
            if (WIFEXITED(state) == 0)
            {
                pResponse->setStatusCode(INTERNAL_SERVER_ERROR);

                pRequest->cgiRunning = false;
                return;
            }
            pResponse->getTempFile()._close();
            pResponse->getTempFile()._open();
            pResponse->readFromCgi();
            pRequest->cgiRunning = false;
        }

    }
}
