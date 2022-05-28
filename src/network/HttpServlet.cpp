//
// Created by b0n3 on 4/17/22.
//


#include <csignal>
#include "HttpServlet.h"




void HttpServlet::addServer(std::string name, Server *server) {
    if (server == nullptr  || name != server->getHost())
        throw IllegalArgumentException("invalid server");
    if (this->servers.find(name) != this->servers.end())
        throw IllegalArgumentException("server already exists");
    this->servers[name] = server;

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
    int i = 1;
    setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(int));
    int set = 1;
    //setsockopt(this->sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set, sizeof(int));
    signal(SIGPIPE, SIG_IGN);
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    if (bind(sock, (struct sockaddr *) &address, sizeof(address)) == -1) {
        throw IllegalStateException("port :" + std::to_string(this->port) + " is already in use");
    }
    if (listen(sock, 1000) == -1) { // to change to max connection
        throw IllegalStateException("port :" + std::to_string(this->port) + " is already in use");
    }
    std::cout << "HttpServlet started on port :" << this->port << std::endl;
    struct pollfd pfd = {};

    pfd.fd = sock;
    pfd.events = POLLIN;
    this->pollfd_list.push_back(pfd);

}

HttpServlet::HttpServlet(int port) {
    this->port = port;

}
void HttpServlet::acceptNewClient(struct pollfd pfd) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_sock;

    if (pfd.revents & POLLIN)
    {
        client_sock = accept(sock, (struct sockaddr *) &client, &client_len);
        fcntl(client_sock, F_SETFL, O_NONBLOCK);
        if (client_sock == -1) {
            perror("accept");
            return;
        }
            struct pollfd clientPfd = {};
            clientPfd.fd = client_sock;
            clientPfd.events = POLLIN;
            this->pollfd_list.push_back(clientPfd);

    }
}

void HttpServlet::handleRequests() {

    int size = this->pollfd_list.size();

    if (this->pollfd_list[0].revents & POLLIN)
        this->acceptNewClient(this->pollfd_list[0]);
    std::vector<int> to_delete;
    for (int i = 1; i < size; i++) {
        int fd = this->pollfd_list[i].fd;
        if (this->requests.find(fd)!= this->requests.end() && this->requests[fd]->cgiRunning) {
            this->handleRequest(this->requests[fd],
                                this->responses[fd]);
            if (!requests[fd]->cgiRunning)
                this->pollfd_list[i].events = POLLOUT;
            continue;
        }

        if (pollfd_list[i].revents & POLLIN) {
            if (this->requests.find(fd) == this->requests.end()) {
                this->requests[fd] = new HttpRequest(fd);
                this->responses[fd] = new HttpResponse();
                if (this->requests[fd]->IsFinished()) {
                    this->handleRequest(this->requests[fd],
                                        this->responses[fd]);
                    if (!requests[fd]->cgiRunning)
                       this->pollfd_list[i].events = POLLOUT;
                }
                continue;
            }

            if (this->requests[fd]->IsFinished()) {
                    this->handleRequest(this->requests[fd],
                                        this->responses[fd]);
                if (!requests[fd]->cgiRunning)
                    this->pollfd_list[i].events = POLLOUT;
                    continue;
                }

            if (!this->requests[fd]->IsFinished()) {
                this->requests[fd]->Parse();
                if (this->requests[fd]->IsFinished()) {
                    this->handleRequest(this->requests[fd],
                                        this->responses[fd]);
                    if (!requests[fd]->cgiRunning)
                        this->pollfd_list[i].events = POLLOUT;
                    continue;
                }
            }
        }
       else  if (pollfd_list[i].revents & POLLOUT) {
            // @ todo check if the request is still not served
            if (!this->responses[fd]->isFinished()) {
                this->responses[fd]->writeToFd(fd);
                pollfd_list[i].events = POLLOUT;
            }
            else
                to_delete.push_back(fd);

          //  std::cout << close(fd) << std::endl;
       }
    }
    for (int i = 0; i < to_delete.size(); i++)
    {
            for ( int j = 0; j < this->pollfd_list.size(); j++)
            {
                if (to_delete[i] == this->pollfd_list[j].fd)
                {

                    int fd = to_delete[i];
                    std::map<int , HttpResponse *>::iterator
                    response = this->responses.find(fd);
                    std::map<int , HttpRequest *>::iterator
                            req = this->requests.find(fd);
                    this->requests.erase(req);
                    this->responses.erase(response);
                    this->pollfd_list.erase(this->pollfd_list.begin() + j);
                    close(fd);
                    break;
                }
            }
        }

}



void HttpServlet::handleRequest(HttpRequest *request, HttpResponse *response, std::string server) {
    Server *s = this->servers[server];
    Location *l = s->getLocation(request->GetPath());
    if (response->getStatusCode() == 200) {
        if (this->servers.find(server) == this->servers.end()) {
            response->setBody("Server not found");
            response->setStatusCode(BAD_GATEWAY);

        }
        if (request->GetPath().empty() || request->GetPath() == " ") {
            response->setStatusCode(MOVED_PERMANENTLY);
            response->addHeader("Location", "/");
            return;
        }

        if (std::atoi(request->GetHeadersValueOfKey("Content-Length").c_str()) > s->getMaxBodySize()) { // @todo change this one to
            response->setStatusCode(BAD_REQUEST);
        }
        if (l == nullptr) {
            response->setStatusCode(NOT_FOUND);
            return;
        }
        request->root = s->getRoot();
//    request->SetPath(request->GetPath().substr(l->getRout().size()));
        request->location = l->getRout();
        if (l->isAllowedMethod(request->GetMethod())) {
            if (l->getCgiIfExists(request->GetPath()))
                l->handleCgi(request, response);
            else
                l->handleStatic(request, response);

        } else {
            response->setStatusCode(METHOD_NOT_ALLOWED);
        }
    }
    if (response->getStatusCode() >= 400) {
        Page *p = nullptr;
        int fd = -1;
        for (int i = 0; i < l->getErrorPages().size(); i++) {
            p = l->getErrorPages()[i]->isInThisPage(response->getStatusCode());
            if (p != nullptr) {
                fd = p->openFile();
                break;
            }
        }
        if (fd != -1 && p != nullptr)
        {
            response->getTempFile().setFd(fd);
            response->setContentLength(countFileSize(p->getContentPath().c_str()));
            response->setContentType(getContentTypeFromFileName(p->getContentPath()));
        }
    }
    // @Todo find error page
}

void HttpServlet::handleRequest(HttpRequest *request, HttpResponse *response) {
    std::string server = request->GetHeadersValueOfKey("Host");

    if (request->getStatusCode() != 200)
    {
        response->setStatusCode(request->getStatusCode());
        response->setBody("Bad Request");

    }
    if (server.empty()) {
        response->setStatusCode(BAD_REQUEST);
        std::string body = "<html><body><h1>400 Bad Request</h1></body></html>";
        response->setBody(body);

    }
    this->handleRequest(request, response, server);
}





