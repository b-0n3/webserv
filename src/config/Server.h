//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_SERVER_H
#define WEBSERV_SERVER_H

#include <string>
#include <vector>
#include "Location.h"
#include "../container/Node.h"
#include "Token.h"
#include "../tools/Utils.h"
#include "Redirect.h"
#include "../network/HttpRequest.h"


class Server {
private:

    std::string host;
    int port;
    std::string root;
    std::vector<Location *> locations;
    bool autoIndex;
    std::string uploadDir;
    std::vector<std::string> indexFiles;
    std::vector<Page *> errorPages;
    std::vector<Cgi *> cgis;
    std::string rootRir;
    std::vector<std::string> allowedMethods;
    std::vector<Redirect *> redirects;

    typedef void (Server::*func)(Node<Token *> *);

    std::map<std::string, func> parsingMethods;
    long long maxBodySize;
    long timeOut;
public:
    long getTimeOut() const;

    void setTimeOut(long timeOut);

    Server();

    ~Server();

    Server(std::string host, int port, std::string root);

    void initLocations();

    std::string getHost();

    int getPort();

    Redirect *getRedirect(std::string path);

    std::string getRoot();

    void initParsingMethods();

    void parseHost(Node<Token *> *node);

    void parsePort(Node<Token *> *node);

    void parseRoot(Node<Token *> *node);

    void parseLocation(Node<Token *> *node);

    void parseAutoIndex(Node<Token *> *node);

    void parseUploadDir(Node<Token *> *node);

    void parseIndexFiles(Node<Token *> *node);

    void parseRedirect(Node<Token *> *node);

    void parseErrorPages(Node<Token *> *node);

    void parseCgi(Node<Token *> *node);

    void parseAllowedMethods(Node<Token *> *node);

    void parseMaxBodySize(Node<Token *> *node);

    void parseTimeOut(Node<Token *> *node);

    const std::vector<std::string> &getAllowedMethods() const;

    void setAllowedMethods(const std::vector<std::string> &allowedMethods);

    std::vector<Location *> getLocations();

    bool requestBelongToThisServer(std::string host, int port);

    Location *getLocation(std::string path);

    static Server *fromNode(Node<Token *> *root);

    void addIndexFile(String indexFile);

    void addAllowedMethod(String method);

    void addCgi(Cgi *cgi);

    long long getMaxBodySize(HttpRequest *request);

    void addErrorPage(Page *page);

    void setHost(const std::string &host);

    void setPort(int port);

    void setRoot(const std::string &root);

    void setLocations(const std::vector<Location *> &locations);

    bool isAutoIndex() const;

    void setAutoIndex(std::string autoIndex);

    const std::string &getUploadDir() const;

    void setUploadDir(const std::string &uploadDir);

    const std::vector<std::string> &getIndexFiles() const;

    void setIndexFiles(const std::vector<std::string> &indexFiles);

    const std::vector<Page *> &getErrorPages() const;

    void setErrorPages(const std::vector<Page *> &errorPages);

    const std::vector<Cgi *> &getCgis() const;

    void setCgis(const std::vector<Cgi *> &cgis);

    const std::string &getRootRir() const;

    void setRootRir(const std::string &rootRir);

    long long getMaxBodySize() const;

    void setMaxBodySize(long long maxBodySize);

    long getTimeOutAt(HttpRequest *request);
};


#endif //WEBSERV_SERVER_H
