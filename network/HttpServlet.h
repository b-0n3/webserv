//
// Created by b0n3 on 4/17/22.
//

#ifndef WEBSERV_HTTPSERVLET_H
#define WEBSERV_HTTPSERVLET_H
#include <string>
#include <map>
#include <vector>
#include "HttpRequest.h"
#include "HttpResponse.h"


class HttpServlet {
private:
    int port;
    std::map<std::string, std::string> servers;
    void handleRequest(HttpRequest request, HttpResponse response);
    void handleRequest(HttpRequest request, HttpResponse response, std::string server);
public:
    HttpServlet(int port);
    void addServer(std::string name, std::string ip);
    void removeServer(std::string name);
    void start();
    void stop();

};


#endif //WEBSERV_HTTPSERVLET_H
