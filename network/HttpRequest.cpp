
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
                                   BodyParsed(false),
                                   StatusCode(200) {

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
        StatusCode = 400;
    this->request.append(buffer, ret);
}

void    HttpRequest::Parse() {
    
    
    int ret = 0;
    // To Check if this correct
    if ((ret = read(Socketfd, buffer, 5000)) < 0)
        StatusCode = 400;

    this->request.append(buffer, ret);
    //if header is finished and not parsed
    if (IsHeaderFinished() && !IsHeaderParsed()) {
        
        //std::cout << request << std::endl;
    
        Method = strtok((char *) request.c_str(), " ");
        Path = strtok(NULL, " ");

        //If there is Host with path "GET http://wwww.1337.ma/index.html HTTP/1.1"
        if (Path.find("http://") != std::string::npos) {
            Path = Path.substr(7);//to skip http://
            //set Host: 
            SetHeaders("Host", Path.substr(0, Path.find("/")));
            Path = Path.substr(Path.find(" "));
        }
        
        //If there is Params with path "GET http://wwww.1337.ma/index.html?user=razaha&promo=2019 HTTP/1.1"
        if (Path.find("?") != std::string::npos){
            std::string params = Path.substr(Path.find("?") + 1);
            Path = Path.substr(0, Path.find("?"));
            //parse params
            while (1)
            {
                std::string token = params.substr(0, params.find("&"));
                SetParams(token.substr(0, token.find("=")), token.substr(token.find("=") + 1));
                if (params.find("&") == std::string::npos)
                {
                    SetParams(token.substr(0, token.find("=")), token.substr(token.find("=") + 1));
                    break;
                }
                params = params.substr(Path.find("&") + 1);
            }
        }

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


