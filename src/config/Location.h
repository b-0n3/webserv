//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_LOCATION_H
#define WEBSERV_LOCATION_H

#include <vector>
#include <string>
#include <iostream>
#include "Page.h"
#include <algorithm>
#include "Cgi.h"
#include "../tools/Utils.h"
#include "../container/Node.h"
#include "Token.h"
#include "../network/HttpRequest.h"
#include "../network/HttpResponse.h"
#include "../network/StatusCode.h"
class Location;
typedef std::string String;
typedef  void (Location::*func)(Node<Token *> *);
class Location {
private:
    std::string route;
    std::vector<std::string> allowedMethods;
    bool autoIndex;
    std::string uploadDir;
    std::vector<std::string> indexFiles;
    std::vector<Page*> errorPages;
    std::vector<Cgi*> cgis;
    bool stripPrefix;
    std::string rootRir;
    bool autoIndexParsed;

    std::map<std::string, func> parsingMethods;
    unsigned  long maxBodySize;
public:
    void setAutoIndex1(bool autoIndex);

    bool isStripPrefix() const;
    ~Location();
    void setStripPrefix(std::string &stripPrefix);

    const std::map<std::string, func> &getParsingMethods() const;

    void setParsingMethods(const std::map<std::string, func> &parsingMethods);

    unsigned long getMaxBodySize() const;

    void setMaxBodySize(unsigned long maxBodySize);

public:
    Location();
    void initParsingMethods();
    void parseRoot(Node<Token *> *node);
    void parseStripPrefix(Node<Token *> *node);
    void parseAutoIndex(Node<Token*> *node);
    void parseUploadDir(Node<Token*> *node);
    void parseIndexFiles(Node<Token*> *node);
    void parseErrorPages(Node<Token*> *node);
    void parseCgi(Node<Token*> *node);
    void parseAllowedMethods(Node<Token*> *node);
    void parseMaxBodySize(Node<Token*> *node);
    std::string getRout() const;

    void setRoute(const std::string &rout);

    const std::vector<std::string> &getAllowedMethods() const;

    const std::string &getRootRir() const;

    void setRootRir(const std::string &rootRir);

    void setAllowedMethods(const std::vector<std::string> &allowedMethods);

    const std::string &getRoute() const;

    bool isAutoIndex() const;

    const std::string &getUploadDir() const;
    void setAutoIndex(String uploadDir) ;
    void setUploadDir(const std::string &uploadDir);
    std::string getIndexFile(String dir);
    const std::vector<std::string> &getIndexFiles() const;

    void setIndexFiles(const std::vector<std::string> &indexFiles);
    const std::vector<Page *> &getErrorPages() const;
    void setErrorPages(const std::vector<Page *> &errorPages);
    const std::vector<Cgi *> &getCgis() const;
    void setCgis(const std::vector<Cgi *> &cgis);
    bool isAllowedMethod(const std::string &method) const;
    bool isInLocation(const std::string &path) const;
    Cgi  *getCgiIfExists(const std::string &path) const;

    static Location *fromNode(Node<Token *> *root);
    void addIndexFile(String indexFile);
    void addAllowedMethod(String method);

    void addCgi(Cgi *cgi);

    void addErrorPage(Page *page);

    void handleStatic(HttpRequest *pRequest, HttpResponse *pResponse);

    void handleCgi(HttpRequest *pRequest, HttpResponse *pResponse);

    void handleGet(HttpRequest *req, HttpResponse *res);
    void handlePost(HttpRequest *req, HttpResponse *res);
    void handleDelete(HttpRequest *req, HttpResponse *res);

    bool is_file(std::string path);

    void redirect(HttpRequest *req, HttpResponse *res, std::string path);
};


#endif //WEBSERV_LOCATION_H
