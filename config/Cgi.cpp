//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include <fstream>
#include "Cgi.h"
#include "../network/StatusCode.h"


Cgi::Cgi(std::string  path, std::vector<std::string>  ext) {
  std::ifstream file(path.c_str(), std::ios::in |std::ios::app);
  if (!file.is_open()) {
      throw FileNotFoundException("cgi File not found");
  }
  file.close();
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
std::string headerToEnv(std::pair<std::string, std::string > header)
{
    std::string env = "HTTP_";
    for (int i = 0; i < header.first.size(); i++)
    {
        if (header.first[i] == '-')
            env += '_';
        else
            env += toupper(header.first[i]);
    }
    env += "=";
    env += header.second;
    return env;
}
const char **createEnv(HttpRequest *request)
{
    std::vector<std::string> envp_vect;

    envp_vect.push_back("REQUEST_URI=" + request.getPath().substr(0, request.getPath().find_last_of("/")));                                             /* =/cgi-bin/php/hello.php */
    envp_vect.push_back("DOCUMENT_URI=" + request.getPath() + request.getQueryString());                                            /* =/cgi-bin/php/hello.php */
    envp_vect.push_back("SCRIPT_NAME=" +  request.getPath());                                             /* =/cgi-bin/php/hello.php */
    envp_vect.push_back("SCRIPT_FILENAME=" + response.get_full_path());                                         /* =/APP/examples/cgi-bin/php/hello.php */
    envp_vect.push_back("PATH_TRANSLATED=" + response.get_full_path());                                         /* =/APP/examples/cgi-bin/php/hello.php */
    envp_vect.push_back("QUERY_STRING=" + request.getQueryString());                                            /* = */
    envp_vect.push_back("SERVER_NAME=localhost" /*+ request.getHeaders(response.get_server()[response.getServerIndex()].get_server_names()[response.getServerIndex()])*/);                                             /* =localhost */
    envp_vect.push_back("SERVER_PORT=" + std::to_string(request.getPort()));                                             /* =82 */
    envp_vect.push_back("REQUEST_METHOD=" + request.getMethod());                                          /* =GET */
    envp_vect.push_back("DOCUMENT_ROOT=" + response.get_root());                                           /* =/APP/examples */
    envp_vect.push_back("GETAWAY_INTERFACE=CGI/1.1");                                       /* =CGI/1.1 */
    envp_vect.push_back("SERVER_PROTOCOL=HTTP/1.1");                                         /* =HTTP/1.1 */
    envp_vect.push_back("REDIRECT_STATUS=200");                                         /* =200 */
    envp_vect.push_back("FCGI_ROLE=RESPONDER");                                               /* =RESPONDER */
    envp_vect.push_back("REQUEST_SCHEME=http");                                          /* =http */
    envp_vect.push_back("SERVER_SOFTWARE=webserv/1.1");                                         /* =webserv/1.1 (Linux) */
    envp_vect.push_back("PATH=" + std::string(std::getenv("PATH")));                                                    /* =/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin */
    envp_vect.push_back("REMOTE_ADDR=0.0.0.0");                                             /* =0.0.0.0 */
    envp_vect.push_back("REMOTE_PORT=0");                                             /* =0 */

    if (request.get_contentLength() > 0)
    {
        envp_vect.push_back("CONTENT_TYPE=" + request.getHeaders("Content-Type"));
        envp_vect.push_back("CONTENT_LENGTH=" + std::to_string(request.get_contentLength()));
    }
    else
    {
        envp_vect.push_back("CONTENT_TYPE=text/html; charset=UTF-8");
        envp_vect.push_back("CONTENT_LENGTH=0");
    }
    for (std::vector<string_pair>::iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); it++)
        envp_vect.push_back(header_to_envp(*it));

    _envp = (char**)malloc(sizeof(char*) * (envp_vect.size() + 1));
    size_t i = 0;
    while (i < envp_vect.size())
    {
        _envp[i] = strdup(envp_vect[i].c_str());
        std::cout << "envp: [" << _envp[i] << "]" << std::endl;
        i++;
    }
    _envp[i] = NULL;
}

// @todo: write a function that will return a Cgi object from a node
void Cgi::execute(HttpRequest *pRequest, HttpResponse *pResponse) {
    std::cout << "Cgi::execute" << std::endl;
   const char  *args[3];
    args[0] =  this->binaryPath.c_str();
    args[1] =  pRequest->GetPath().c_str();
    args[0] = nullptr;
    const char **env;
    pResponse->setStatusCode(FOUND);
}
