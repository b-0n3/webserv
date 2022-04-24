//
// Created by b0n3 on 4/17/22.
//

#include <unistd.h>
#include <iostream>
#include "HttpResponse.h"
HttpResponse::HttpResponse() {
    this->statusCode = 200;
    this->contentLength = 0;
    this->contentType = "text/html";
}
void HttpResponse::writeToFd(int i) {


    write(i, "HTTP/1.1 ", 9);
    write(i, std::to_string(this->statusCode).c_str(),
          std::to_string(this->statusCode).length());
    write(i, " OK\r\n", 5);
    write(i, "Content-Length: ", 16);
    write(i, std::to_string(this->contentLength).c_str(),
          std::to_string(this->contentLength).length());
    write(i, "\r\n", 2);
    for (std::map<std::string, std::string>::iterator it = this->headers.begin();
    it != this->headers.end(); ++it){
        write(i, it->first.c_str(),
              it->first.length());
        write(i, ": ", 2);
        write(i, it->second.c_str(),
              it->second.length());
        write(i, "\r\n", 2);
    }
    write(i, "\r\n", 2);
  write(i, body.c_str(), body.length());
   // std::cout << "Wrote to fd size => " << ret  <<"contentLe :" << this->contentLength<< std::endl;
}

void HttpResponse::addHeader(std::string const &key, std::string const  &value) {
    this->headers[key] = value;
}

int HttpResponse::getStatusCode() {
    return statusCode;
}

std::string HttpResponse::getStatusMessage() {
    return std::string();
}

std::string HttpResponse::getContentType() {
    return contentType;
}

std::string HttpResponse::getBody() {
    return body;
}

std::map<std::string, std::string> HttpResponse::getHeaders() {
    return headers;
}

void HttpResponse::setStatusMessage(std::string statusMessage) {

}

void HttpResponse::setHeaders(std::map<std::string, std::string> headers) {
    this->headers = headers;
}

void HttpResponse::setBody(std::string const &body) {
    this->body = body;
    this->contentLength = body.length();
}

void HttpResponse::setStatusCode(int statusCode) {
    this->statusCode = statusCode;
}

void HttpResponse::setContentType(std::string contentType) {
    this->contentType = contentType;
}




