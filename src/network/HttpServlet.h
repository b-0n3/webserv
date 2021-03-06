

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
#include "StatusCode.h"
#include <cstring>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../exceptions/IllegalStateException.h"
#include "../tools/Utils.h"

class HttpServlet {
private:
    int port;

    struct sockaddr_in address;
    int sock;
    std::map<std::string, Server *> servers;
    std::map<int, HttpRequest *> requests;
    std::map<int, HttpResponse *> responses;

    void acceptNewClient(struct pollfd pfd);

    void handleRequest(HttpRequest *request, HttpResponse *response);

    void handleRequest(HttpRequest *request, HttpResponse *response, std::string server);

public:
    std::vector<struct pollfd> pollfd_list;

    ~HttpServlet();

    HttpServlet(int port);

    void addServer(std::string name, Server *server);

    unsigned long long getMaxBodySize(HttpRequest *request);

    void start();

    void handleRequests();

    long getTimeOut(HttpRequest *pRequest);
};


#endif //WEBSERV_HTTPSERVLET_H
