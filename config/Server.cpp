//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include "Server.h"

Server::Server(std::string host, int port, std::string root) {
    this->host = host;
    this->port = port;
    this->root = root;

}

std::string Server::getHost() {
    return this->host;
}

int Server::getPort() {
    return this->port;
}

std::string Server::getRoot() {
    return std::string();
}

std::vector<Location> Server::getLocations() {
    return this->locations;
}


bool Server::requestBelongToThisServer(std::string host, int port) {
    return this->host == host && this->port == port;
}

Location *Server::getLocation(std::string path) {
    for (std::vector<Location>::iterator it = this->locations.begin();
    it != this->locations.end();
    ++it) {
        if (it->isInLocation(path)) {
            return &*it;
        }
    }
    return nullptr;
}
