//
// Created by b0n3 on 4/17/22.
//

#ifndef WEBSERV_HTTPREQUEST_H
#define WEBSERV_HTTPREQUEST_H
#include <string>
#include <map>


class HttpRequest {
private:
    std::string method;
    std::string path;
    std::string version;
    std::string body;
    bool readingBody;
    bool finished;
    unsigned  long contentLength;
    unsigned  long bodyReaded;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    HttpRequest();
    void parse(std::string request);
    void setHeader(std::string key, std::string value);
    void setBody(std::string body);
    void setFinished(bool finished);
    void setContentLength(unsigned long contentLength);
    void setMethod(std::string method);
    void setPath(std::string path);
    void setVersion(std::string version);
    void readFromFd(int fd);
public:
    static HttpRequest *fromFd(int fd);
    void continueReadFromFd(int fd);
    std::string getMethod();
    std::string getPath();
    std::string getVersion();
    std::string getBody();
    bool isFinished();
    unsigned long getContentLength();
    std::string getHeader(std::string key);
    std::string getParam(std::string key);
    std::map<std::string, std::string> getHeaders();
    std::map<std::string, std::string> getParams();
};


#endif //WEBSERV_HTTPREQUEST_H
