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
    for( int i = 0; i< 100; i++)
        this->free_sock.push(i);
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


}

HttpServlet::HttpServlet(int port) {
    this->port = port;

}

void HttpServlet::handleRequests() {

        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int client_sock = accept(sock, (struct sockaddr *) &client, &client_len);

        if (client_sock == -1) {
           return;
        }
        std::string host = inet_ntoa(client.sin_addr);
        std::cout << "host : " << host << std::endl;
        if (this->free_sock.empty()){
            this->wait_sock.push(client_sock);
        }
        else {
            if (!this->wait_sock.empty())
            {
                int s = this->wait_sock.top();
                this->wait_sock.pop();
                int id = this->free_sock.top();
                this->free_sock.pop();
                this->pollfds[id].fd = s;
                this->pollfds[id].events = POLLIN;
                this->pollfds[id].revents = 0;
                this->used_sock.push_back(id);
            }
            if (!this->free_sock.empty()) {

                int id = this->free_sock.top();
                this->free_sock.pop();
                this->used_sock.push_back(id);
                this->pollfds[id].fd = client_sock;
                this->pollfds[id].events = POLLIN;
                this->pollfds[id].revents = 0;
            }
            else
            {
                this->wait_sock.push(client_sock);
            }
        }
        int sd = poll(this->pollfds, 100, -1);
        if (sd == -1)
            return;
        for (int i = 0; i < this->used_sock.size(); i++) {
            int index = this->used_sock[i];
            if (this->pollfds[index].revents & POLLIN) {
                if (this->requests.find(this->pollfds[index].fd) == this->requests.end()) {
                    this->requests[this->pollfds[index].fd] = HttpRequest::fromFd(this->pollfds[index].fd);
                    this->responses[this->pollfds[index].fd] = new HttpResponse();
                }
                if (this->requests.find(this->pollfds[index].fd) != this->requests.end()) {
                    HttpRequest *request = this->requests[this->pollfds[index].fd];
                    HttpResponse *response = this->responses[this->pollfds[index].fd];
                    if (request->isFinished()) {
                        this->handleRequest(request, response, request->getHeader("Host"));
                    }
                    else{
                        request->continueReadFromFd(this->pollfds[index].fd);
                        if (request->isFinished())
                            this->pollfds[index].events = POLLOUT;
                    }
                }
            }else if (this->pollfds[i].revents & POLLOUT) {
                // todo: write response
                HttpResponse *response = this->responses[this->pollfds[index].fd];
                response->writeToFd(this->pollfds[index].fd);
                /// todo: clean up
                close(this->pollfds[i].fd);
                this->free_sock.push(index);
                this->used_sock.erase(this->used_sock.begin() + i);
                this->requests.erase(this->pollfds[index].fd);
                this->responses.erase(this->pollfds[index].fd);

            }

        }
}

void HttpServlet::handleRequest(HttpRequest *request, HttpResponse *response, std::string server) {
    Server *s = this->servers[server];
   Location *l =  s->getLocation(request->getPath());
    if (l == nullptr) {
        response->setStatusCode(NOT_FOUND);
        response->setBody("404 Not Found");
        return;
    }
   if (l->isAllowedMethod(request->getMethod())) {
        if (l->getCgiIfExists(request->getPath()))
            l->handleCgi(request, response);
        else
            l->handleStatic(request, response);

   }
   else {
       response->setStatusCode(METHOD_NOT_ALLOWED);
       response->setBody("405 Method Not Allowed");
   }
}


