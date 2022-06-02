//
// Created by Abdelouahad Ait hamd on 5/29/22.
//

#ifndef WEBSERV_REDIRECT_H
#define WEBSERV_REDIRECT_H
#include <string>
#include <map>
#include "../container/Node.h"
#include "Token.h"
#include "../tools/Utils.h"
#include "../exceptions/IllegalArgumentException.h"
class Redirect {
private:
    std::string url;
    int  status;
    std::string location;
    typedef void (Redirect::*func)(Node<Token*> *);
    std::map<std::string, func> parseFunctions;
    void initParseFunctions();
    void parseUrl(Node<Token*> *);
    void parseStatus(Node<Token*> *);

public:
    void setUrl(const std::string &url);

    void setStatus(int status);

    void setLocation(const std::string &location);

private:
    void parseLocation(Node<Token*> *);

public:
    Redirect();
    std::string &getUrl();
    int  getStatus();
    std::string &getLocation();
    static Redirect *fromNode(Node<Token *> *node);


};


#endif //WEBSERV_REDIRECT_H
