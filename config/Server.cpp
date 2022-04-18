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
    return this->root;
}

std::vector<Location *> Server::getLocations() {
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
    if (root->getChildren().empty())
        throw IllegalArgumentException("empty server not allowed");
    s = new Server();
    for(int i = 0; i < root->getChildren().size(); i++)
    {
        if (root->getChildren()[i]->getData()->getValue() == "server_name") {
            s->host = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        } else if (root->getChildren()[i]->getData()->getValue() == "listen") {
            s->port = std::stoi(root->getChildren()[i]->getChildren()[0]->getData()->getValue());
        } else if (root->getChildren()[i]->getData()->getValue() == "root") {
            s->root = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
            std::cout << "fsdf" << s->root << std::endl;
        } else if (root->getChildren()[i]->getData()->getValue() == "locations") {
            for (int j = 0; j < root->getChildren()[i]->getChildren().size();j ++) {
                s->locations.push_back(Location::fromNode(root->getChildren()[i]->getChildren()[j]));
            }
        }
        else if (root->getChildren()[i]->getData()->getValue() == "client_max_body_size") {
            if (!is_digits(root->getChildren()[i]->getChildren()[0]->getData()->getValue()))
                throw IllegalArgumentException("client_max_body_size must be a number");
            s->maxBodySize = std::stoi(root->getChildren()[i]->getChildren()[0]->getData()->getValue());
        }
        else
            throw IllegalArgumentException("unexpected token" + root->getChildren()[i]->getData()->getValue());
    }
    return s;
}

Server::Server() {
    this->host = "localhost";
    this->port = 80;
    this->root = "/var/www/html";
    this->maxBodySize = 1024;
}
