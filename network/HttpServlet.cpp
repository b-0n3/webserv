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
    for( int i = 1023; i > 0; i--) {
        this->free_sock.push(i);
        this->pollfds[i].fd = -1;
        this->pollfds[i].events = POLLIN;
    }
}
HttpServlet::~HttpServlet() {
    while(close(this->sock) == -1);
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
    if (listen(sock, 128) == -1) { // to change to max connection
        throw IllegalStateException("port :" + std::to_string(this->port) + " is already in use");
    }
    std::cout << "HttpServlet started on port :" << this->port << std::endl;
    this->pollfds[0].fd = sock;
    this->pollfds[0].events = POLLIN;

 //   this->pollfds[0].revents = POLLIN;


}

HttpServlet::HttpServlet(int port) {
    this->port = port;

}


void HttpServlet::acceptNewClient() {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_sock;

    if (this->pollfds[0].revents & POLLIN)
    {
        client_sock = accept(sock, (struct sockaddr *) &client, &client_len);
        if (client_sock == -1)
                 return;

            fcntl(client_sock, F_SETFL, O_NONBLOCK);
            if (this->free_sock.empty())
            {
                this->wait_sock.push(client_sock);
            }
            else if (!this->wait_sock.empty())
            {
                    int s = this->wait_sock.top();
                    int id  = this->free_sock.top();
                    this->free_sock.pop();
                    this->wait_sock.pop();
                    this->pollfds[id].fd = s;
                    this->pollfds[id].events = POLLIN;
                    this->used_sock.push_back(id);
            }else
            {
                int id = this->free_sock.top();
                this->free_sock.pop();
                this->pollfds[id].fd = client_sock;
                this->pollfds[id].events = POLLIN;
                this->used_sock.push_back(id);
            }

    }
}


void HttpServlet::handleRequests() {

    int nfds = poll(this->pollfds, this->used_sock.size() + 1, 1);
    if (nfds == -1)
        return;
    if (nfds == 1 && this->pollfds[0].revents & POLLIN)
    {
        acceptNewClient();
        return;
    }else
        this->acceptNewClient();
    std::vector<int> to_delete;
    for (int i = 0; i < this->used_sock.size(); i++) {
        int id = this->used_sock[i];
        int fd = this->pollfds[id].fd;
        if (this->pollfds[id].revents & POLLIN) {
            if (this->requests.find(fd) == this->requests.end()) {
                this->requests[fd] = new HttpRequest(fd);
                this->responses[fd] = new HttpResponse();
                if (this->requests[fd]->IsFinished()) {
                    this->handleRequest(this->requests[fd],
                                        this->responses[fd],
                                        "127.0.0.1");
                    this->pollfds[id].events = POLLOUT;
                }
               // continue;
            }

            if (this->requests[fd]->IsFinished()) {
                    this->handleRequest(this->requests[fd],
                                        this->responses[fd],
                                        "127.0.0.1");
                    this->pollfds[id].events = POLLOUT;
                  //  continue;
                }

            if (!this->requests[fd]->IsFinished()) {
                this->requests[fd]->Parse();
                if (this->requests[fd]->IsFinished()) {
                    this->handleRequest(this->requests[fd],
                                        this->responses[fd],
                                        "127.0.0.1");
                    this->pollfds[id].events = POLLOUT;
                  //  continue;
                }
            }
        }
       else  if (this->pollfds[id].revents & POLLOUT) {
//            if (this->responses.find(fd) != this->responses.end()) {
                this->responses[fd]->writeToFd(fd);
                to_delete.push_back(fd);
          //  }
            close(fd);
       }


    }
    for (int i = 0; i < to_delete.size(); i++)
    {
            for ( int j = 0; j < this->used_sock.size(); j++)
            {
                if (to_delete[i] == this->pollfds[this->used_sock[j]].fd)
                {


                    int id = this->used_sock[j];
                    int fd = to_delete[i];
                    std::map<int , HttpResponse *>::iterator
                    response = this->responses.find(fd);
                    std::map<int , HttpRequest *>::iterator
                            req = this->requests.find(fd);
                    this->requests.erase(req);
                    this->responses.erase(response);
                    this->free_sock.push(id);
                    this->pollfds[id].fd = -1;
                   // close(fd);
                    this->used_sock.erase(this->used_sock.begin() + j);
                    break;
                }
            }
        }

//        if (checkRead(this->sock)) {
//            client_sock = accept(sock, (struct sockaddr *) &client, &client_len);
//            if (client_sock >= 0) {
//                fcntl(client_sock, F_SETFL, O_NONBLOCK);
//                std::string host = inet_ntoa(client.sin_addr);
//                //   std::cout << "host : " << host << "fd" << client_sock << std::endl;
//                if (this->requests.size() >= 50)
//                    this->wait_sock.push(client_sock);
//                else {
//
//                    if (this->requests.find(client_sock) == this->requests.end()) {
////                    int id = this->free_sock.top();
////                    this->free_sock.pop();
//                        this->used_sock.push_back(client_sock);
////                    this->pollfds[id].fd = client_sock;
////                    this->pollfds[id].events = POLLIN;
//                    }
//                }
//            }
//        }
//            if (this->requests.size() < 50 && !this->wait_sock.empty())
//            {
//                int s = this->wait_sock.top();
//                this->wait_sock.pop();
//                this->used_sock.push_back(s);
//            }
////
//        if (this->used_sock.empty())
//            return;
//       std:: cout << "used sock size : " << this->used_sock.size() << std::endl;
//        std::vector<int> to_delete;
//     //   std::cout <<"used_sock size : " << nfds << std::endl;
//        for (int i = 0; i < this->used_sock.size(); i++) {
//            int id = this->used_sock[i];
//            int fd = id;
//            if (checkRead(id))
//            {
//                if (this->requests.find(fd) == this->requests.end())
//                {
//                    this->requests[fd] = new HttpRequest(fd);
//                    this->responses[fd] = new HttpResponse();
////                    this->pollfds[id].events = POLLOUT;
//                    if (this->requests[fd]->IsFinished())
//                    {
//                        this->handleRequest(this->requests[fd], this->responses[fd], "127.0.0.1");
//                        //this->pollfds[id].events = POLLOUT;
//                    }
//                  continue;
//                }
//                if(!this->requests[fd]->IsFinished())
//                {
//                    this->requests[fd]->Parse();
//                    if (this->requests[fd]->IsFinished())
//                    {
//                        this->handleRequest(this->requests[fd], this->responses[fd], "127.0.0.1");
//                      //  this->pollfds[id].events = POLLOUT;
//                    }
//                   continue;
//                }
//                if (this->requests[fd]->IsFinished())
//                {
//                    this->handleRequest(this->requests[fd], this->responses[fd], "127.0.0.1");
//                  //  this->pollfds[id].events = POLLOUT;
//                }
//
//            }
//            if (checkWrite(fd))
//            {
//                if (this->responses.find(fd) != this->responses.end())
//                {
//                    this->responses[fd]->writeToFd(fd);
//                    to_delete.push_back(id);
//                    close(fd);
//                }
//
//            }
//        }
//
//        for (int i = 0; i < to_delete.size(); i++)
//        {
//            for ( int j = 0; j < this->used_sock.size(); j++)
//            {
//                if (to_delete[i] == this->used_sock[j])
//                {
//
//                    int fd = to_delete[i];
//                   std::map<int , HttpResponse *>::iterator
//                    response = this->responses.find(fd);
//                    std::map<int , HttpRequest *>::iterator
//                            req = this->requests.find(fd);
//                    this->requests.erase(req);
//                    this->responses.erase(response);
//                    this->used_sock.erase(this->used_sock.begin() + j);
//                    break;
//                }
//            }
//        }


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
//std::cout << "root path : " << s->getRoot() << std::endl;
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



