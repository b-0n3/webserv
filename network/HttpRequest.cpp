
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
    return ((request.find("\r\n\r\n") != std::string::npos) ? true : false) ;
}

bool HttpRequest::IsBodyFinished()
{
    return ((request.find("0\r\n\r\n", request.find("\r\n\r\n") + 4) != std::string::npos) ? true : false) ;
}

void    HttpRequest::Parse() {
    
    
    int ret = 0;
    // To Check if this correct
    if ((ret = read(Socketfd, buffer, 5000)) < 0)
    {
        StatusCode = 400;
        return;
    }

  //  std::cout << "buffer: " << buffer << std::endl;
    this->request.append(buffer, ret);
    std::cout << "request: " << request << std::endl;
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
            std::string token = strtok(NULL, "\n");
          //  std::cout << token << std::endl;
            if (token[0] == '\r' || token[1] == '\n')
                break;
            SetHeaders(token.substr(0, token.find(":")), token.substr(token.find(":") + 2));
        }
        SetHeaderParsed(true);
    }

    //if has body and header parsed and body not parsed yet
    if (IsHeaderParsed() && IsHasBody() && IsBodyFinished()  && !IsBodyParsed())
    {
        std::cout << "heeeeel: " << Body << std::endl;
        if (GetHeadersValueOfKey("Transfer-Encoding") == "chunked")
        {
			std::string body = request.substr(request.find("\r\n\r\n") + 2);
			strtok((char *)body.c_str(), "\r\n");
			for (size_t i = 0; true; i++)
			{
				std::string token = strtok(NULL, "\r\n");
				if (!(i % 2))
					Body.append(token.substr(0, token.find("\r\n")));
				if (token[0] == '\r' && token[1] == '\r' && token[2] == '\n')
					break;
			}        
        }
        if (GetHeadersValueOfKey("Transfer-Encoding") == "gzip" && GetHeadersValueOfKey("Content-Encoding") == "gzip")
        {
            ;
        }
        else
            Body.append(request.substr(request.find("\r\n\r\n") + 4));

        std::cout << std::endl << std::endl << "Body: " << Body << std::endl;
		IsBodyEqualContentLenght() ? SetBodyParsed(true) : (void)(StatusCode = 400);
    }
}
