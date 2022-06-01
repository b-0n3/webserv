//
// Created by b0n3 on 4/17/22.
//

#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/stat.h>
#include "HttpResponse.h"
#include "StatusCode.h"
#include "filesystem"
#include "../tools/Utils.h"

HttpResponse::HttpResponse() :finished(false) , headerWrited(0), writingBody(false),bodySkiped(0),chunked(false) {
    this->statusCode = 200;
    this->contentLength = 0;
    this->cgiHeaderSize = 0;
    this->responseBuilted = false;
    this->contentType = "text/html";
}
void HttpResponse::writeToFd(int i) {



    if (!writingBody) {
        if (!this->responseBuilted)
            this->buildResponseHeaders();
    std::cout << "writing headers" << std::endl;
    std::cout << this->responseHeadersString.str() << std::endl;
    std::string toWrite = responseHeadersString.str().substr(headerWrited);

           int ret = write(i, toWrite.c_str(), toWrite.size());
           if (ret == -1) {
               std::cout << "Error writing to socket" << std::endl; // todo : log warning
               return;
           }
           this->headerWrited += ret;
            if (this->headerWrited >= responseHeadersString.str().length())
                writingBody = true;
         }

    // if (!writingBody) {
    //     write(i, "HTTP/1.1 ", 9);
    //     write(i, std::to_string(this->statusCode).c_str(), std::to_string(this->statusCode).length());
    //     write(i, " OK\r\n", 5); // todo add custum message
    //     write(i, "Content-Length: ", 16);
    //     write(i, std::to_string(this->contentLength).c_str(), std::to_string(this->contentLength).length());
    //     write(i, "\r\n", 2);
    //     write(i, "Connection: close", strlen("Connection: close"));
    //     write(i, "\r\n", 2);
    //     for (int j  = 0; j < this->cookies.size(); j++)
    //     {
    //      //   std::cout << "writing cookie = "<< this->cookies[j] << std::endl;
    //         write(i, "Set-Cookie: ",12);
    //         write(i, this->cookies[j].c_str(),this->cookies[j].length());
    //         write(i, "\r\n",2);
    //     }
    //     for (std::map<std::string, std::string>::iterator it = this->headers.begin();
    //          it != this->headers.end(); ++it) {
    //         write(i, it->first.c_str(),
    //               it->first.length());
    //         write(i, ": ", 2);
    //         write(i, it->second.c_str(),
    //               it->second.length());
    //         write(i, "\r\n", 2);
    //     }
    //     write(i, "\r\n", 2);
    //     if (this->bodySkiped > 0) {
    //         write(i, body.c_str(), body.length());
    //     }
    //     writingBody = true;
    // }
    if (this->contentLength > 0) {
        int BUFFER_SIZE = 20000;
        char buff[BUFFER_SIZE];
        int ret = read(this->getBodyFileDescriptor(), buff, BUFFER_SIZE);

        if (ret <= 0)
            this->finished = true;
        else
            write(i, buff, ret);
        if (ret < BUFFER_SIZE && ret <= this->contentLength)
            this->finished = true;
    }
    else
        this->finished = true;
  //  std::cout << "Wrote to fd size => " << ret  <<"contentLe :" << this->contentLength<< std::endl;
}

void HttpResponse::addHeader(std::string const &key, std::string const  &value) {
    if (key == "Set-Cookie" || key == "set-cookie") {
        this->cookies.push_back(value);
        std::cout <<value<< std::endl;
    }
    else
        this->headers[key]  =  value;
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

std::map<std::string, std::string,compareStringIgnoreCase> HttpResponse::getHeaders() {
    return headers;
}

void HttpResponse::setStatusMessage(std::string statusMessage) {

}

void HttpResponse::setHeaders(std::map<std::string, std::string,compareStringIgnoreCase> headers) {
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

unsigned long HttpResponse::getContentLength() const {
    return contentLength;
}

void HttpResponse::setContentLength(unsigned long contentLength) {
    HttpResponse::contentLength = contentLength;
}

int HttpResponse::getCgiReadFd() const {
    return cgiReadFd;
}

void HttpResponse::setCgiReadFd(int cgiReadFd) {
    HttpResponse::cgiReadFd = cgiReadFd;
}

void HttpResponse::readFromCgi() {
    char buf[1024];
    int ret;
    std::string bd;
    while ((ret = read(this->getBodyFileDescriptor(), buf, 1024)) > 0) {
        bd.append(buf, ret);
        if (bd.find("\r\n\r\n") != std::string::npos)
            break;
        write(1, buf, ret);
    }
    this->setStatusCode(OK);
    std::string headers = bd.substr(0, bd.find("\r\n\r\n") + 4);
    this->parseHeaders(headers);
    this->body = bd.substr(bd.find("\r\n\r\n") + 4);
    this->bodySkiped = this->body.size();
    this->cgiHeaderSize = headers.size();
    this->contentLength =  countFileSize(this->getTempFile().getFileName().c_str());
    std::cerr << this->contentLength << " content length" << std::endl;
    this->contentLength -= headers.length();
    std::cerr << headers.length() << std::endl;
}

void HttpResponse::parseHeaders(std::string &headers) {
 std::string firstLine = headers.substr(0, headers.find("\r\n"));
    std::string header = headers.substr(headers.find("\r\n") + 2);
    std::string key;
    std::string value;
    if (firstLine.find("Status") != std::string::npos) {
        std::string sc = firstLine.substr(firstLine.find(' ') + 1,
                                                   firstLine.find(' ') + 3);
        this->setStatusCode(std::stoi(sc));
    } else {
        key = firstLine.substr(0, firstLine.find(':'));
       value = firstLine.substr(firstLine.find(':') + 1);
        this->addHeader(key, value);
        this->setStatusCode(OK);
    }

    while (header.length() > 0) {
        std::string nextHeader = header.substr(0, header.find("\r\n"));
        header = header.substr(header.find("\r\n") + 2);
        key = nextHeader.substr(0, nextHeader.find(':'));
        value = nextHeader.substr(nextHeader.find(':') + 1);
        this->addHeader(key, value);
    }
}

HttpResponse::~HttpResponse() {
    tempFile.deleteFile();
    std::cout << "deleted" << std::endl;
}

void HttpResponse::buildResponseHeaders() {

    std::cout << "build response headers" << std::endl;

    responseHeadersString << "HTTP/1.1 ";


    responseHeadersString << std::to_string(this->statusCode);
    //write(i, std::to_string(this->statusCode).c_str(), std::to_string(this->statusCode).length());
    responseHeadersString << " ";
    responseHeadersString << this->statusCodeToString(this->statusCode);
    responseHeadersString << "\r\n";
    //write(i, " OK\r\n", 5); // todo add custum message

    responseHeadersString << "Content-Length: ";
    //write(i, "Content-Length: ", 16);

    responseHeadersString << std::to_string(this->contentLength);
    //write(i, std::to_string(this->contentLength).c_str(), std::to_string(this->contentLength).length());

    responseHeadersString << "\r\n";
    //write(i, "\r\n", 2);
    
                    //responseHeadersString << "Content-Type: text/html\r\n"; // ADDED IN DEBUGGING

    responseHeadersString << "Connection: close"; // @todo add keep alive
    //write(i, "Connection: close", strlen("Connection: close"));

    responseHeadersString << "\r\n";
    //write(i, "\r\n", 2);

    for (int j  = 0; j < this->cookies.size(); j++)
    {
        //   std::cout << "writing cookie = "<< this->cookies[j] << std::endl;

        responseHeadersString << "Set-Cookie: ";
        //write(i, "Set-Cookie: ",12);

        responseHeadersString << this->cookies[j];
        //write(i, this->cookies[j].c_str(),this->cookies[j].length());

        responseHeadersString << "\r\n";
        //write(i, "\r\n",2);
    }
    for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
        responseHeadersString << it->first;
        //write(i, it->first.c_str(),it->first.length());

        responseHeadersString << ": ";
        //write(i, ": ", 2);

        responseHeadersString << it->second;
        //write(i, it->second.c_str(),it->second.length());

        responseHeadersString << "\r\n";
        //write(i, "\r\n", 2);
    }

    responseHeadersString << "\r\n";
    //write(i, "\r\n", 2);


    if (this->bodySkiped > 0) {
        responseHeadersString << body;
        //write(i, body.c_str(), body.length());
    }
    this->responseBuilted =true;
}

std::string HttpResponse::statusCodeToString(int statusCode) {
    std::map<int, std::string> statusCodes;
    statusCodes[OK] = "OK";
    statusCodes[NOT_FOUND] = "Not Found";
    statusCodes[BAD_REQUEST] = "Bad Request";
    statusCodes[METHOD_NOT_ALLOWED] = "Method Not Allowed";
    statusCodes[INTERNAL_SERVER_ERROR] = "Internal Server Error";
    statusCodes[NOT_IMPLEMENTED] = "Not Implemented";
    statusCodes[BAD_GATEWAY] = "Bad Gateway";
    statusCodes[SERVICE_UNAVAILABLE] = "Service Unavailable";
    statusCodes[GATEWAY_TIMEOUT] = "Gateway Timeout";
    statusCodes[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
    statusCodes[LENGTH_REQUIRED] = "Length Required";
    statusCodes[PRECONDITION_FAILED] = "Precondition Failed";
    statusCodes[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
    statusCodes[FOUND] = "Found";
    statusCodes[SEE_OTHER] = "See Other";
    statusCodes[TEMPORARY_REDIRECT] = "Temporary Redirect";

    statusCodes[BAD_GATEWAY] = "Bad Gateway";
    statusCodes[SERVICE_UNAVAILABLE] = "Service Unavailable";
    statusCodes[GATEWAY_TIMEOUT] = "Gateway Timeout";
    statusCodes[HTTP_VERSION_NOT_SUPPORTED] = "HTTP Version Not Supported";
    statusCodes[LENGTH_REQUIRED] = "Length Required";
    statusCodes[PRECONDITION_FAILED] = "Precondition Failed";
    statusCodes[UNSUPPORTED_MEDIA_TYPE] = "Unsupported Media Type";
    if (statusCodes.find(statusCode) != statusCodes.end()) {
        return statusCodes[statusCode];
    }
    return "Unknown";
}





