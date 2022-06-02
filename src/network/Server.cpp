//
// Created by Abdelouahad Ait hamd on 4/18/22.
//
#include <set>
#include <sys/stat.h>
#include "../config/ConfigParser.h"
#include "HttpServlet.h"
#include "../exceptions/IllegalStateException.h"

std::set<int> getPorts(std::vector<Server *> servers) {

    std::set<int> ports;
    for (unsigned long i = 0; i < servers.size(); i++) {
         ports.insert(servers[i]->getPort());
    }
    return ports;
}
void initTmpDir()
{
    std::string tmpDir = getenv("PWD");
    tmpDir += "/.tmp";


    if( is_directory(tmpDir.c_str()))
        return;

    if (mkdir(tmpDir.c_str(), 0777) == -1) {
            throw IllegalStateException("Could not create tmp directory");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2)
    {
         Logger::log(Logger::LOG_LEVEL_ERROR, "Usage: " + std::string (argv[0]) + " configFile Path");
        exit(1);
    }

    std::vector<Server *> servers;
    std::vector<HttpServlet *> servlets;
    try {
        initTmpDir();
        ConfigParser configFileParser(argv[1]);
        configFileParser.tokenizeConfigFiles(nullptr, nullptr, -1, -1);
        servers = configFileParser.validateAst();
        if (servers.empty())
            throw IllegalArgumentException("No server found in the config file");
        // @Todo: init The httpServlets and add A loop to handle all the requests
        std::set<int> ports = getPorts(servers);
        for (std::set<int>::iterator  it =
                ports.begin();
                it != ports.end();
                 it++) {
           HttpServlet *s = new HttpServlet(*it);

           for (unsigned long j = 0; j < servers.size(); j++) {
               if (servers[j]->getPort() == *it) {
                   try {
                       s->addServer(servers[j]->getHost(),
                                    servers[j]);
                   }catch (std::exception &e)
                   {
                       delete s;
                       throw e;
                   }
               }
           }
           servlets.push_back(s);
        }
        for (unsigned long i = 0; i < servlets.size(); i++) {
            servlets[i]->start();
        }
    }
    catch (std::exception &e) {
        Logger::log(Logger::LOG_LEVEL_DEBUG, "error while parsing config file");

    exit(1);
    }
    while (true) {
            try {
                // @Todo : use poll here to handle the requests
                std::vector<struct pollfd> pList;
                for (unsigned long i = 0; i < servlets.size(); i++)
                    for (unsigned long j = 0; j < servlets[i]->pollfd_list.size(); j++)
                        pList.push_back(servlets[i]->pollfd_list[j]);
                struct pollfd *pfds = convertToArray(pList);
                poll(pfds, pList.size(), 10);
                for (unsigned long i = 0; i < pList.size(); i++)
                    for (unsigned long j = 0; j < servlets.size(); j++)
                        for (unsigned long k = 0; k < servlets[j]->pollfd_list.size(); k++) {
                            if (pfds[i].fd == servlets[j]->pollfd_list[k].fd) {
                                servlets[j]->pollfd_list[k].revents = pfds[i].revents;
                                break;
                            }
                        }
                delete[] pfds;
                for (unsigned long i = 0; i < servlets.size(); i++) {
                    servlets[i]->handleRequests();

                }
            }
            catch (std::exception &e) {
                Logger::log(Logger::LOG_LEVEL_ERROR, "Runtime error");
                return 1;
            }
        }
   return 0;
}