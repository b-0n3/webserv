
#include "Redirect.h"


Redirect::Redirect() {
    this->url = "";
    this->status = 0;
    this->location = "";
    this->initParseFunctions();
}

std::string &Redirect::getUrl() {
    return url;
}

int Redirect::getStatus() {
    return status;
}

std::string &Redirect::getLocation() {
    return location;
}

Redirect *Redirect::fromNode(Node<Token *> *node) {
    Redirect *redirect = new Redirect();
    std::string value = node->getData()->getValue();
    try {


        if (value.empty())
            throw IllegalArgumentException("Redirect url empty");
        func f = redirect->parseFunctions["url"];
        (redirect->*f)(node);

        if (node->getChildren().empty() || node->getChildren().size() != 2) {
            throw IllegalArgumentException("Redirect node must have 2 children");
        }
        for (size_t i = 0; i < node->getChildren().size(); i++) {
            value = node->getChildren()[i]->getData()->getValue();
            if (redirect->parseFunctions
                        .find(value) == redirect->parseFunctions.end())
                throw IllegalArgumentException(value + "unexpected token  in redirect");
            f = redirect->parseFunctions[value];
            (redirect->*f)(node->getChildren()[i]);
        }
    }
    catch (std::exception &e) {
        delete redirect;
        throw e;
    }
    return redirect;
}

void Redirect::initParseFunctions() {
    this->parseFunctions["url"] = &Redirect::parseUrl;
    this->parseFunctions["status"] = &Redirect::parseStatus;
    this->parseFunctions["location"] = &Redirect::parseLocation;
}

void Redirect::parseUrl(Node<Token *> *n) {
    std::string value;
    if (!this->url.empty())
        throw IllegalArgumentException("Redirect url already set");

    if (n->getChildren().size() != 2)
        throw IllegalArgumentException("Redirect url must have 1 child");
    value = n->getData()->getValue();
    if (value.empty())
        throw IllegalArgumentException("Redirect url empty");
    this->url = value;
}

void Redirect::parseStatus(Node<Token *> *n) {
    std::string value;
    int allowdRedirectStatusCodes[] = {301, 302, 303, 307};
    if (this->status != 0)
        throw IllegalArgumentException("Redirect status already set");
    if (n->getChildren().size() != 1)
        throw IllegalArgumentException("Redirect status must have 1 child");
    value = n->getChildren()[0]->getData()->getValue();
    if (value.empty())
        throw IllegalArgumentException("Redirect status empty");
    this->status = std::stoi(value);
    if (std::find(std::begin(allowdRedirectStatusCodes),
                  std::end(allowdRedirectStatusCodes), this->status)
        == std::end(allowdRedirectStatusCodes))
        throw IllegalArgumentException("Redirect status not allowed");
}

void Redirect::parseLocation(Node<Token *> *n) {
    if (!this->location.empty())
        throw IllegalArgumentException("Redirect location already set");
    if (n->getChildren().size() != 1)
        throw IllegalArgumentException("Redirect location must have 1 child");
    std::string value = n->getChildren()[0]->getData()->getValue();
    if (value.empty())
        throw IllegalArgumentException("Redirect location empty");
    this->location = value;
}

void Redirect::setUrl(const std::string &url) {
    Redirect::url = url;
}

void Redirect::setStatus(int status) {
    Redirect::status = status;
}

void Redirect::setLocation(const std::string &location) {
    Redirect::location = location;
}

