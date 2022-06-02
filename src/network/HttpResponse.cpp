
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sstream>
#include <sys/stat.h>
#include "HttpResponse.h"
#include "StatusCode.h"
#include "filesystem"
#include "../tools/Utils.h"
#include "../tools/Logger.h"

HttpResponse::HttpResponse() {
    this->statusCode = 200;
    this->contentLength = 0;
    headerWrited = 0;
    this->bodyWrited = 0;
    finished = false;
    writingBody = false;
    bodySkiped = 0;
    chunked = false;
    this->cgiHeaderSize = 0;
    this->responseBuilted = false;
    this->contentType = "text/html";
}

void HttpResponse::writeToFd(int i) {


    if (!writingBody) {
        this->body = "";
        if (!this->responseBuilted)
            this->buildResponseHeaders();
        std::string toWrite = responseHeadersString.str().substr(headerWrited);

        int ret = write(i, toWrite.c_str(), toWrite.size());
        if (ret == -1) {
            Logger::log(Logger::LOG_LEVEL_WARNING, " can not write  from socket");
            return;
        }
        this->headerWrited += ret;
        if (this->headerWrited >= responseHeadersString.str().length())
            writingBody = true;
    }


    if (this->contentLength > 0) {
        size_t readSize = 25000;
        char buff[readSize];
        size_t writeRet;
        size_t ret = read(this->getBodyFileDescriptor(), buff, readSize);

        if (ret < 0 || (this->bodyWrited == 0 && ret == 0)) 
            this->finished = true;
        else {
            body.append(buff, ret);
            writeRet = write(i, body.c_str(), body.size());
            if (writeRet < 0) {
                this->finished = true;
                Logger::log(Logger::LOG_LEVEL_ERROR, std::string("could not write  to socket"));
                return;
            }
            this->bodyWrited += writeRet;
            body = body.substr(writeRet);

        }
        if (this->bodyWrited >= this->contentLength)
            this->finished = true;
    } else
        this->finished = true;

}

void HttpResponse::addHeader(std::string const &key, std::string const &value) {
    if (key == "Set-Cookie" || key == "set-cookie") {
        this->cookies.push_back(value);

    } else
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

std::map<std::string, std::string, compareStringIgnoreCase> HttpResponse::getHeaders() {
    return headers;
}


void HttpResponse::setHeaders(std::map<std::string, std::string, compareStringIgnoreCase> headers) {
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
    this->contentLength = countFileSize(this->getTempFile().getFileName().c_str());
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

}

void HttpResponse::buildResponseHeaders() {


    responseHeadersString << "HTTP/1.1 ";


    responseHeadersString << std::to_string(this->statusCode);

    responseHeadersString << " ";
    responseHeadersString << this->statusCodeToString(this->statusCode);
    responseHeadersString << "\r\n";

    responseHeadersString << "Content-Length: ";

    responseHeadersString << std::to_string(this->contentLength);

    responseHeadersString << "\r\n";

    if (this->contentLength < 20000)
        responseHeadersString << "Connection: close"; // @todo add keep alive
    else
        responseHeadersString << "Connection: keep-alive";

    responseHeadersString << "\r\n";

    for (unsigned long j = 0; j < this->cookies.size(); j++) {

        responseHeadersString << "Set-Cookie: ";

        responseHeadersString << this->cookies[j];

        responseHeadersString << "\r\n";
    }
    for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
        responseHeadersString << it->first;

        responseHeadersString << ": ";

        responseHeadersString << it->second;

        responseHeadersString << "\r\n";
    }

    responseHeadersString << "\r\n";


    if (this->bodySkiped > 0) {
        responseHeadersString << body;
    }
    this->responseBuilted = true;
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

std::string HttpResponse::log() {
    std::stringstream ss;

    ss << " ";
    ss << this->statusCode;
    if (this->statusCode >= 300 && this->statusCode <= 399 && this->headers.count("Location") > 0)
        ss << " " + headers["location"];

    return ss.str();
}





