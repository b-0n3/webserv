//
// Created by b0n3 on 4/17/22.
//

#ifndef WEBSERV_HTTPRESPONSE_H
#define WEBSERV_HTTPRESPONSE_H

#include <string>
#include <map>
#include <vector>
#include "../tools/TempFile.h"
#include "../tools/Utils.h"
class HttpResponse {
private:
    int statusCode;
    unsigned  long contentLength;
    std::string contentType;
    std::string body;
    bool chunked;
    bool writingBody;
    TempFile tempFile;
    bool finished;
    int bodySkiped;
    std::map<std::string, std::string,compareStringIgnoreCase> headers;
    std::vector<std::string> cookies;
    unsigned  long cgiHeaderSize;
    int cgiReadFd;
public:
    unsigned long getContentLength() const;
    void setContentLength(unsigned long contentLength);
    int getCgiReadFd() const;
    void setCgiReadFd(int cgiReadFd);

public:
    HttpResponse();
    ~HttpResponse();
    void readFromCgi();
    int getStatusCode();
    std::string getStatusMessage();
    std::string getContentType();
    std::string getBody();
    bool isWritingBody(){
        return this->writingBody;
    };
    void setChunked(bool chunked){this->chunked = chunked;};
    bool isChunked(){return chunked;};
    void setFinished(bool finished){this->finished = finished;};
    bool isFinished(){return finished;};
    int getBodyFileDescriptor(){return tempFile.getFd();};
    TempFile &getTempFile(){return tempFile;}
    std::map<std::string, std::string,compareStringIgnoreCase> getHeaders();
    void setStatusCode(int statusCode);
    void setStatusMessage(std::string statusMessage);
    void setContentType(std::string contentType);

    void setBody(std::string const &body);

    void setHeaders(std::map<std::string, std::string,compareStringIgnoreCase> headers);

    void addHeader(std::string const &key, std::string  const &value);

    void writeToFd(int i);

    void parseHeaders(std::string &headers);
};


#endif //WEBSERV_HTTPRESPONSE_H
