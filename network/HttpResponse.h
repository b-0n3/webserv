//
// Created by b0n3 on 4/17/22.
//

#ifndef WEBSERV_HTTPRESPONSE_H
#define WEBSERV_HTTPRESPONSE_H

#include <string>
#include <map>
class HttpResponse {
private:
    int statusCode;
    unsigned  long contentLength;
    std::string contentType;
    std::string body;
    std::map<std::string, std::string> headers;
public:
    HttpResponse();
    HttpResponse(int statusCode, std::string statusMessage, std::string contentType, std::string body);
    HttpResponse(int statusCode, std::string statusMessage, std::string contentType, std::string body, std::map<std::string, std::string> headers);
    int getStatusCode();
    std::string getStatusMessage();
    std::string getContentType();
    std::string getBody();
    std::map<std::string, std::string> getHeaders();
    void setStatusCode(int statusCode);
    void setStatusMessage(std::string statusMessage);
    void setContentType(std::string contentType);
    void setBody(std::string body);
    void setHeaders(std::map<std::string, std::string> headers);

    void writeToFd(int i);
};


#endif //WEBSERV_HTTPRESPONSE_H
