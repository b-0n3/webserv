//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_SERVER_H
#define WEBSERV_SERVER_H
#include <string>
#include <vector>
#include "Location.h"


class Server {
private:
    std::string host;
    int port;
    std::string root;
    unsigned  long maxBodySize;
    std::vector<Location> locations;
public:
    Server(std::string host, int port, std::string root);
    std::string getHost();
    int getPort();
    std::string getRoot();
    std::vector<Location> getLocations();
    bool requestBelongToThisServer(std::string host, int port);
    Location *getLocation(std::string path);

};


#endif //WEBSERV_SERVER_H
