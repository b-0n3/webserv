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
    for( int i = 99; i>= 0; i--)
        this->free_sock.push(i);
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
    for (int i = 0; i < 100; i++){
        this->pollfds[i].fd = -1;
        this->pollfds[i].events = POLLIN;
    }
//    this->pollfds[0].fd = this->sock;
//    this->pollfds[0].events = POLLIN;
 //   this->pollfds[0].revents = POLLIN;


}

HttpServlet::HttpServlet(int port) {
    this->port = port;

}

void HttpServlet::handleRequests() {
    static int nfds;
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);

    int client_sock = accept(sock, (struct sockaddr *) &client, &client_len);

    if (client_sock == -1) {
        return;
    }
    fcntl(client_sock, F_SETFL, O_NONBLOCK);
    std::string host = inet_ntoa(client.sin_addr);
    std::cout << "host : " << host << std::endl;

    if (std::count(this->used_sock.begin(), this->used_sock.end(), client_sock) == 0) {
        pollfd pollf;
        pollf.fd = client_sock;
        pollf.events = POLLIN;
        this->used_pollfds.push_back(pollf);
        this->used_sock.push_back(client_sock);
    }


    int size = this->used_pollfds.size();
    pollfd *pollf = this->used_pollfds.data();
  //  std::copy(this->used_pollfds.begin(), this->used_pollfds.end(), pollf);
    int s = poll(pollf, size, -1);
    if (s == -1)
        return;
    std::vector<int> deletedSocket;
//    for (int i = 0; i < size; i++) {
//        deletedSocket.push_back(this->used_sock[i]);
//    }

    for (int i = 0; i < size; i++) {
        if (pollf[i].revents & POLLIN) {
           // todo parse Request;
           if (this->requests.find(pollf[i].fd)== this->requests.end()) {
               this->requests[pollf[i].fd] = new HttpRequest(pollf[i].fd);
               this->responses[pollf[i].fd] = new HttpResponse();
            //   continue;
           }
           if(!this->requests[pollf[i].fd]->IsHeaderFinished())
           {
               this->requests[pollf[i].fd]->ContinueParse();
             //  continue;
           }
           if (this->requests[pollf[i].fd]->IsHeaderFinished())
           {
               this->handleRequest(this->requests[pollf[i].fd], this->responses[pollf[i].fd], "127.0.0.1");
              std::cout << this->used_pollfds[i].fd << " sdf " << pollf[i].fd << std::endl;
               this->used_pollfds[i].events = POLLOUT;
              // continue;
           }

        }else if (pollf[i].revents & POLLOUT){
            // todo write message to client;
            this->responses[pollf[i].fd]->writeToFd(pollf[i].fd);
            close(this->used_pollfds[i].fd);
            this->used_pollfds[i].fd = -1;
            deletedSocket.push_back(i);
        }
    }

    for (int i = 0; i < deletedSocket.size(); i++)
        this->used_pollfds.erase(this->used_pollfds.begin() + deletedSocket[i]);
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



