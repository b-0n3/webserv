//
// Created by b0n3 on 4/17/22.
//

#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "HttpServlet.h"
#include <cstring>
#include "../exceptions/IllegalStateException.h"

void HttpServlet::addServer(std::string name, Server *server) {
    if (server == nullptr  || name != server->getHost())
        throw IllegalArgumentException("invalid server");
    this->servers[name] = server;
    for( int i = 99; i >= 0; i--) {
        this->free_sock.push(i);
        this->pollfds[i].fd = -1;
        this->pollfds[i].events = POLLIN;
    }
}
HttpServlet::~HttpServlet() {
 close(this->sock);
}
void HttpServlet::start() {

    address.sin_family = AF_INET;
    address.sin_port = htons(this->port);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    fcntl( sock, F_SETFL, O_NONBLOCK);
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) == -1) {
        throw IllegalStateException("port :" + std::to_string(this->port) + " is already in use");
    }
    if (listen(sock, 100) == -1) { // to change to max connection
        throw IllegalStateException("port :" + std::to_string(this->port) + " is already in use");
    }
    std::cout << "HttpServlet started on port :" << this->port << std::endl;


 //   this->pollfds[0].revents = POLLIN;


}

HttpServlet::HttpServlet(int port) {
    this->port = port;

}
bool checkRead(int sock){
    pollfd pfd[1];
    pfd[0].fd = sock;
    pfd[0].events = POLLIN;
    int r = poll(pfd, 1, 0);
    if (r == -1)
        return false;
    return pfd[0].revents & POLLIN;
}



void HttpServlet::handleRequests() {
    static int nfds;
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_sock;

        if (checkRead(sock)){
            client_sock = accept(sock, (struct sockaddr *) &client, &client_len);
            if (client_sock >=0) {
                fcntl(client_sock, F_SETFL, O_NONBLOCK);
                std::string host = inet_ntoa(client.sin_addr);
                std::cout << "host : " << host << "fd" << client_sock << std::endl;
            }

}



void HttpServlet::handleRequest(HttpRequest *request, HttpResponse *response, std::string server) {
    Server *s = this->servers[server];
    Location *l = s->getLocation(request->GetPath());
    if (l == nullptr) {
        response->setStatusCode(NOT_FOUND);
        std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
        response->setBody(body);
        return;
    }
    if (l->isAllowedMethod(request->GetMethod())) {
//        request->SetPath(s->getRoot() + request->GetPath());
std::cout << "root path : " << s->getRoot() << std::endl;
        l->setRootRir(s->getRoot());
        if (l->getCgiIfExists(request->GetPath()))
            l->handleCgi(request, response);
        else
            l->handleStatic(request, response);

    } else {
        response->setStatusCode(METHOD_NOT_ALLOWED);
        std::string body = "<html><body><h1>405 Method Not Allowed</h1></body></html>";
        response->setBody(body);
    }
}



