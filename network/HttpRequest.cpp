//
// Created by b0n3 on 4/17/22.
//

#include <unistd.h>
#include "HttpRequest.h"

HttpRequest::HttpRequest() {
    this->contentLength = 0;
    this->finished = false;
}

void HttpRequest::parse(std::string request) {

}

void HttpRequest::setHeader(std::string key, std::string value) {

}

void HttpRequest::setBody(std::string body) {

}

void HttpRequest::setFinished(bool finished) {

}

void HttpRequest::setContentLength(unsigned long contentLength) {

}

void HttpRequest::setMethod(std::string method) {

}

void HttpRequest::setVersion(std::string version) {

}

void HttpRequest::readFromFd(int fd) {

}

std::string HttpRequest::getMethod() {
    return std::string();
}

std::string HttpRequest::getPath() {
    return std::string();
}

std::string HttpRequest::getVersion() {
    return std::string();
}

std::string HttpRequest::getBody() {
    return std::string();
}

bool HttpRequest::isFinished() {
    return false;
}

unsigned long HttpRequest::getContentLength() {
    return 0;
}

std::string HttpRequest::getHeader(std::string key) {
    return std::string();
}

std::string HttpRequest::getParam(std::string key) {
    return std::string();
}

std::map<std::string, std::string> HttpRequest::getHeaders() {
    return std::map<std::string, std::string>();
}

std::map<std::string, std::string> HttpRequest::getParams() {
    return std::map<std::string, std::string>();
}

HttpRequest *HttpRequest::fromFd(int fd) {
    unsigned char buffer[5000];
    int ret;
    while  ( (ret = read(fd, buffer, 5000)) >= 0)
    {

    }
    return nullptr;
}

void HttpRequest::setPath(std::string path) {

}
