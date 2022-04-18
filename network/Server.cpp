//
// Created by Abdelouahad Ait hamd on 4/18/22.
//
#include <set>
#include "../config/ConfigParser.h"
#include "HttpServlet.h"
std::set<int> getPorts(std::vector<Server *> servers) {
    std::set<int> ports;
    for (int i =0; i < servers.size(); i++) {
        ports.insert(servers[i]->getPort());
    }
    return ports;
}
int main(int argc, char *argv[]) {
    if (argc != 2)
    {
        std::cout<< "Usage: " << argv[0] << " configFile Path" << std::endl;
        exit(1);
    }
    try {
        ConfigParser configFileParser(argv[1]);
        configFileParser.tokenizeConfigFiles(nullptr, nullptr, -1, -1);
        std::vector<Server *> servers = configFileParser.validateAst();
        std::vector<HttpServlet > servlets;
        if (servers.empty())
            throw IllegalArgumentException("No server found in the config file");
        // @Todo: init The httpServlets and add A loop to handle all the requests
        std::set<int> ports = getPorts(servers);
        for (int i= 0; i< ports.size(); i++) {
           HttpServlet s = HttpServlet(*ports.find(i));
           for (int j = 0; j < servers.size(); j++) {
               if (servers[j]->getPort() == *ports.find(i)) {
                   s.addServer(servers[j]->getHost(), servers[j]);
               }
           }
           servlets.push_back(s);
        }
            for (int i = 0; i < servlets.size(); i++) {
                servlets[i].start();
            }
            while (true)
            {
                for (int i = 0; i < servlets.size(); i++) {
                    servlets[i].handleRequests();
                }
            }
    }catch ( std::exception &e)
    {
        std::cout << "Error while parsing the config file :" << std::endl << e.what() << std::endl;
        exit(1);
    }
    return 0;
}