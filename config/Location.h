//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#ifndef WEBSERV_LOCATION_H
#define WEBSERV_LOCATION_H

#include <vector>
#include <string>
#include <iostream>
#include "Page.h"
#include "Cgi.h"
#include "../tools/filter.h"
#include "../container/Node.h"
#include "Token.h"

class Location {
private:
    std::string route;
    std::vector<std::string> allowedMethods;

    std::string uploadDir;
    std::vector<std::string> indexFiles;
    std::vector<Page*> errorPages;
    std::vector<Cgi*> cgis;
public:


    std::string getRout() const;

    void setRoute(const std::string &rout);

    const std::vector<std::string> &getAllowedMethods() const;

    void setAllowedMethods(const std::vector<std::string> &allowedMethods);

    const std::string &getUploadDir() const;

    void setUploadDir(const std::string &uploadDir);

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
};


#endif //WEBSERV_LOCATION_H
