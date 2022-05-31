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

void headerToEnv(std::string key, std::string value)
{
    std::string env = "HTTP_";
    for (int i = 0; i < key.size(); i++) {
        if (key[i] == '-')
            env += '_';
        else
            env += std::toupper(key[i]);
    }
    setenv(env.c_str(), value.c_str() , 1);
}

// void headerToEnv(std::map<std::string,std::string>::iterator  header)
// {
//     std::string env = "HTTP_";
//     for (int i = 0; i < (*header).first.size(); i++)
//     {
//         if ((*header).first[i] == '-')
//             env += '_';
//         else
//             env += std::toupper((*header).first[i]);
//     }
//    // env += "=";
//    // env += header.second;
//     setenv(env.c_str(),  (*header).second.c_str(), 1)
// }
void createEnv(HttpRequest *request)
{
  //  std::vector<std::string> envp_vect;
    std::string script_name = request->GetPath();
         //   .substr( request->GetPath().find_last_of('/') );
//    script_name.substr(request->location.size());

    std::string script_path = request->root+
            request->GetPath();
    std::string query_string = getParams(request->GetParams());

    setenv("DOCUMENT_URI",request->getRealPath().c_str(),1);                 /* =/cgi-bin/php/hello.php */
    setenv("REQUEST_URI",(request->getRealPath()+"?" + query_string).c_str() ,1);                                            /* =/cgi-bin/php/hello.php */
    setenv("SCRIPT_NAME",  script_name.c_str(),1);/* =/cgi-bin/php/hello.php */
  //  setenv("PATH_INFO=" +request->GetPath().substr(0, request->GetPath().find_last_of('/') + 1));
    setenv("SCRIPT_FILENAME",script_path.c_str(),1);                                         /* =/APP/examples/cgi-bin/php/hello.php */
    setenv("PATH_TRANSLATED",script_path.c_str(),1);                                         /* =/APP/examples/cgi-bin/php/hello.php */
    setenv("QUERY_STRING" ,query_string.c_str(),1);                                            /* = */
    setenv("SERVER_NAME" ,request->GetHeadersValueOfKey("Host").c_str(),1 /*+ request.getHeaders(response.get_server()[response.getServerIndex()].get_server_names()[response.getServerIndex()])*/);                                             /* =localhost */
    setenv("SERVER_PORT" ,std::to_string(request->port).c_str(),1);                                             /* =82 */
    setenv("REQUEST_METHOD", request->GetMethod().c_str(),1);                                          /* =GET */
    setenv("DOCUMENT_ROOT", request->root.c_str(),1);                                           /* =/APP/examples */
    setenv("GETAWAY_INTERFACE","CGI/1.1",1);                                       /* =CGI/1.1 */
    setenv("SERVER_PROTOCOL","HTTP/1.1",1);                                         /* =HTTP/1.1 */
    setenv("REDIRECT_STATUS","200",1);                                         /* =200 */
    setenv("FCGI_ROLE","RESPONDER",1);                                               /* =RESPONDER */
    setenv("REQUEST_SCHEME","http",1);                                          /* =http */
    setenv("SERVER_SOFTWARE","webserv/1.1",1);                                         /* =webserv/1.1 (Linux) */
    setenv("PATH" , std::getenv("PATH"),1);                                                    /* =/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin */
    setenv("REMOTE_ADDR","0.0.0.0" ,1/*+ request->remoteAddress*/);                                             /* =0.0.0.0 */
    setenv("REMOTE_PORT","0",1  /**+ std::to_string(request->port)*/);                                             /* =0 */

    if (request->IsHasBody() )
    {
        setenv("CONTENT_TYPE" , request->GetHeadersValueOfKey("Content-Type").c_str(),1);
        setenv("CONTENT_LENGTH" , request->GetHeadersValueOfKey("Content-Length").c_str(),1);
    }

   for( std::map<std::string, std::string,compareStringIgnoreCase>::iterator it = request->GetHeaders().begin();
        it != request->GetHeaders().end(); it++)
    {
        std::cerr << (*it).first << " " << (*it).second << std::endl;
        headerToEnv( (*it).first, (*it).second);
    }

//    char **_envp = new char*[envp_vect.size()];
//    size_t i = 0;
//    while (i < envp_vect.size())
//    {
//        _envp[i] = (char *)envp_vect[i].c_str();
//       std::cout << "envp: [" << _envp[i] << "]" << std::endl;
//        i++;
//    }
//    _envp[i] = NULL;


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
        args[2] = nullptr;

        std::cerr << "CGI: " << this->binaryPath << " " << path << std::endl;
        int err = dup(STDERR_FILENO);
        pid_t pid = fork();
        if (pid == -1) {
            pResponse->setStatusCode(INTERNAL_SERVER_ERROR);
            return;
        }
        if (pid == 0) {

            // std::cout << "methode =" << pRequest->GetMethod() << std::endl;
            // std::cout << "Cgi::execute: " << args[0] << " " << args[1] << std::endl;
             dup2(err, STDERR_FILENO);
             createEnv(pRequest);
         //   std::cout << "execve" << std::endl;
            // std::cout << this->binaryPath<< std::endl;
            if (pRequest->IsHasBody()) {
                dup2( pRequest->GetBodyFd(),  STDIN_FILENO);
                dup2(  pResponse->getBodyFileDescriptor() , STDOUT_FILENO);
                close(pRequest->GetBodyFd());
                close(pResponse->getBodyFileDescriptor());

            } else {
               // close(readPipe[0]);
                dup2( pResponse->getBodyFileDescriptor() , STDOUT_FILENO );
                close(pResponse->getBodyFileDescriptor());
                close(0);
             //  dup2(readPipe[1] , STDERR_FILENO );
              // close(readPipe[1]);
            }
          int s =   execvp( this->binaryPath.c_str(), (char *const *) args);
            exit(s);
        } else {
         //   dup2(STDERR_FILENO, out);
          //  delete []   env;
            if (pRequest->IsHasBody()) {
               // close(writePipe[0]);
                    close(pRequest->GetBodyFd());
                //    std::cout << "writing body to cgi" << std::endl;
                   // @Todo: change this to nonBlocking
//                char buff[1056];
//                int ret;
//                while ((ret = read(pRequest->GetBodyFd(), buff, 1056)) >0 )
//                {
//                    write(writePipe[1], buff, ret);
//                    write(1, buff, ret);
//                }
//                close(pRequest->GetBodyFd());
//                close(writePipe[1]);

            }
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
