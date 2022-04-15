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
    for (std::vector<Location *>::iterator it = this->locations.begin();
    it != this->locations.end();
    ++it) {
        if ((*it)->isInLocation(path)) {
            return *it;
        }
    }
    return nullptr;
}

Server *Server::fromNode(Node<Token *> *root) {
    Server *s;

    if (root->getData()->getValue() != "server")
        throw IllegalArgumentException("unexpected token" + root->getData()->getValue());
    for(int i = 0; i < root->getChildren().size(); i++)
    {
        s = new Server();
        if (root->getChildren()[i]->getData()->getValue() == "host") {
            s->host = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        } else if (root->getChildren()[i]->getData()->getValue() == "port") {
            s->port = std::stoi(root->getChildren()[i]->getChildren()[0]->getData()->getValue());
        } else if (root->getChildren()[i]->getData()->getValue() == "root") {
            s->root = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        } else if (root->getChildren()[i]->getData()->getValue() == "location") {
            for (int j = 0; j < root->getChildren()[i]->getChildren().size();j ++) {
                s->locations.push_back(Location::fromNode(root->getChildren()[i]->getChildren()[j]));
            }
        }else
            throw IllegalArgumentException("unexpected token" + root->getChildren()[i]->getData()->getValue());
    }
    return nullptr;
}

Server::Server() {
    this->host = "localhost";
    this->port = 80;
    this->root = "/var/www/html";
    this->maxBodySize = 1024;
}
