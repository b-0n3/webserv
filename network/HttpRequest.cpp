
#include <cstring>
#include <iostream>
#include "HttpRequest.h"

HttpRequest::HttpRequest(int fd) : Socketfd(fd),
                                   buffer(""),
                                   request(""),
                                   Method(""),
                                   Path(""),
                                   Version(""),
                                   Body(""),
                                   HeaderParsed(false),
                                   BodyParsed(false)
{
    Headers.clear(), Params.clear();
    Parse();
}

bool HttpRequest::IsHeaderFinished()
{
    int i = 0;
    while (request[i] != '\0') {
        if (request[i] == '\r' && request[i + 1] == '\n'
            && request[i + 2] == '\r' && request[i + 3] == '\n')
            return true;
        i++;
    }
    return false;
}

void HttpRequest::ContinueParse() {
    int ret = 0;
    //Read And Concatinate The Raw Buffer
    if ((ret = read(Socketfd, buffer, 5000)) < 0)
        throw std::runtime_error("Error while reading from socket");
  //  std::cout << "read: " << ret << std::endl;
    this->request.append(buffer, ret);
}

void    HttpRequest::Parse() {
    int ret;

    // To Check if this correct
    if ((ret = read(Socketfd, buffer, 5000)) < 0)
        throw std::runtime_error("Error while reading from socket");
  //  std::cout << "read: " << ret << std::endl;
    this->request.append(buffer, ret);
    //if header is finished and not parsed
    if (IsHeaderFinished() && !IsHeaderParsed()) {
        //parse header
        std::cout << request << std::endl;
        Method = strtok((char *) request.c_str(), " ");
        Path = strtok(NULL, " ");
      //  std::cout << "path: " << Path << std::endl;
//        if (Path.find_first_of('http://') != std::string::npos) {
//            Path = Path.substr(7);
//        }
        strtok(NULL, "/"); //skip "HTTP/""
        Version = strtok(NULL, "\r\n");

        //parse headers
        while (1) {
           // std::cout << "Header Parsing" << std::endl;

            std::string token = strtok(NULL, "\n");
            std::cout << token << std::endl;
            if (token[0] == '\r' || token[1] == '\n')
                break;
            //Need Syntax check !!
            SetHeaders(token.substr(0, token.find(":")), token.substr(token.find(":") + 2));
        }
        SetHeaderParsed(true);
    }
    //if has body and not parsed
    if (IsHasBody() && !IsBodyParsed())
    {
        //parse body
        SetBodyParsed(true);
    }
}


