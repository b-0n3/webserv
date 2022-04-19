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

//    int sd = poll(this->pollfds,1 , -1);
//    if (sd == -1)
//        return;
//    if (this->pollfds[0].revents & POLLIN) {
        int client_sock = accept(sock, (struct sockaddr *) &client, &client_len);

        if (client_sock == -1) {
            return;
        }
        std::string host = inet_ntoa(client.sin_addr);
        std::cout << "host : " << host << std::endl;
        if (this->free_sock.empty()) {
            this->wait_sock.push(client_sock);
        } else {
            if (!this->wait_sock.empty()) {
                int s = this->wait_sock.top();
                this->wait_sock.pop();
                int id = this->free_sock.top();
                this->free_sock.pop();
                this->pollfds[id].fd = s;
                this->pollfds[id].events = POLLIN;
             //   this->pollfds[id].revents = POLLIN;
                nfds++;
                this->used_sock.push_back(id);
            }
            if (!this->free_sock.empty()) {
                nfds++;
                int id = this->free_sock.top();
                this->free_sock.pop();
                this->used_sock.push_back(id);
                this->pollfds[id].fd = client_sock;
                this->pollfds[id].events = POLLIN;
              //  this->pollfds[id].revents = POLLIN;
            } else {
                this->wait_sock.push(client_sock);
            }
        }

   // }
    int s = poll(this->pollfds,this->used_sock.size() , -1);
    if (s == -1)
        return;
        for (int i = 0; i < this->used_sock.size(); i++) {
            std::cout << "used_sock : " << this->used_sock[i] << std::endl;
            int index = this->used_sock[i];
            HttpRequest *request;
            HttpResponse *response;
            if (this->pollfds[index].revents & POLLIN) {
                if (this->requests.find(this->pollfds[index].fd) == this->requests.end()) {
                    std::cout << "create Request" << std::endl;
                    this->requests[this->pollfds[index].fd] = new HttpRequest(this->pollfds[index].fd);
                    std::cout << "create Request" << std::endl;
                    this->responses[this->pollfds[index].fd] = new HttpResponse();
                }
                if (this->requests.find(this->pollfds[index].fd) != this->requests.end()) {
                    request = this->requests[this->pollfds[index].fd];
                    response = this->responses[this->pollfds[index].fd];
                    ///     std::cout<<"Handle request : "<<request->GetMethod()<<std::endl;
                    if (request->IsHeaderParsed()) {
                        this->handleRequest(request, response,
                                            "127.0.0.1");
                        this->pollfds[index].events = POLLOUT;
                        //this->pollfds[index].revents = POLLOUT;
                        //      response->writeToFd(this->pollfds[index].fd);
                    } else {
                        request->ContinueParse();
                        if (request->IsHeaderParsed()) {
                            this->pollfds[index].events = POLLOUT;
                           // this->pollfds[index].revents = POLLOUT;
                        }
                    }
                    std::cout << "request : " << request->GetMethod() << std::endl;
                }
            }
            else if (this->pollfds[index].revents & POLLOUT) {
                // todo: write response
                std::cout << "write response" << std::endl;
                response->writeToFd(this->pollfds[index].fd);
                /// todo: clean up

                this->free_sock.push(index);
                this->used_sock.erase(this->used_sock.begin() + i);
             delete request;
             delete response;
             this->requests[this->pollfds[index].fd] = nullptr;
             this->responses[this->pollfds[index].fd] = nullptr;
             this->requests.erase(this->pollfds[index].fd);
             this->requests.erase(this->pollfds[index].fd);
//                this->requests[this->pollfds[index].fd] = nullptr;
                //              this->responses[this->pollfds[index].fd] = nullptr;
                close(this->pollfds[index].fd);
                this->pollfds[index].fd = -1;
                this->pollfds[index].events = POLLIN;

                nfds--;

            }
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



