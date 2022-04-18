//
// Created by b0n3 on 4/17/22.
//

#ifndef WEBSERV_HTTPSERVLET_H
#define WEBSERV_HTTPSERVLET_H
#include <string>
#include <map>
#include <vector>
#include <sys/poll.h>
#include <netinet/in.h>
#include <stack>
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "../config/Server.h"


class HttpServlet {
private:
    int port;
    pollfd pollfds[100];
    struct sockaddr_in address;
    int sock;
    std::stack<int> free_sock;
    std::vector<int> used_sock;
    std::stack<int> wait_sock;
    std::map<std::string,  Server * > servers;
    std::map<int , HttpRequest *> requests;
    std::map<int , HttpResponse *> responses;
    void handleRequest(HttpRequest *request, HttpResponse *response);
    void handleRequest(HttpRequest *request, HttpResponse *response, std::string server);
public:
    HttpServlet(int port);
    void addServer(std::string name, Server * server);
    void start();
    void handleRequests();

};


#endif //WEBSERV_HTTPSERVLET_H
