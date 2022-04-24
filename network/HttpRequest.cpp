
#include <cstring>
#include <iostream>
#include "HttpRequest.h"

HttpRequest::HttpRequest(int fd) : Socketfd(fd),
                                   buffer(""),
                                   request(""),
                                   Method(""),
                                   Path(""),
                                   Version(""),
                                   HeaderParsed(false),
                                   BodyParsed(false),
                                   StatusCode(200) {

    Headers.clear(), Params.clear();
	std::string filename = "/tmp/" + std::to_string(rand()) ;
	std::cout << filename << std::endl;
	BodyFd.open(filename);
    Parse();
}

bool HttpRequest::IsHeaderFinished()
{
    return ((request.find("\r\n\r\n") != std::string::npos) ? true : false) ;
}

bool HttpRequest::IsBodyFinished()
{
	;
}

void  HttpRequest::ParseFirstLine(std::string FirstLine)
{
	Method = FirstLine.substr(0, FirstLine.find(' '));
	//std::cout << "Method: " << Method << std::endl;
    
	Path = FirstLine.substr(FirstLine.find(" "), FirstLine.find("HTTP/") - 5 );
	//std::cout << "Path: " << Path << std::endl;

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
		// std::cout << "Path: " << Path << std::endl;

		std::stringstream ss(params);
        //parse params
        while (1)
        {
            std::string token;
			std::getline(ss, token, '&');
			if (token.empty())
				break;
            SetParams(token.substr(0, token.find("=")), token.substr(token.find("=") + 1));
        }
		//std::map<std::string, std::string> paramsss = GetParams();
		// (std::map<std::string, std::string>::iterator it = paramsss.begin(); it != paramsss.end(); ++it)
			//::cout << it->first << " *=> " << it->second << std::endl;
    }

	Version = FirstLine.substr(FirstLine.find("HTTP/") + 5, FirstLine.find("\r\n"));
	//std::cout << "Version: " << Version << std::endl;
}

void  HttpRequest::ParseHeaders(std::string HeadersLine)
{
	std::stringstream ss(HeadersLine);
	
	while (1)
	{
		std::string token;
		std::getline(ss, token, '\n');
		if (token[0] == '\r')
			break;
		SetHeaders(token.substr(0, token.find(":")), token.substr(token.find(":") + 2));
	}
	//std::map<std::string, std::string> headersss = GetHeaders();
	//for (std::map<std::string, std::string>::iterator it = headersss.begin(); it != headersss.end(); ++it)
		//std::cout << it->first << " => " << it->second << std::endl;
}


void check()
{

}

void    HttpRequest::Parse() {
    
    
    int ret = 0;
    // To Check if this correct
    if ((ret = read(Socketfd, buffer, 5000)) < 0)
    {
        StatusCode = 400;
        return;
    }

    if (!IsHeaderParsed())
    	this->request.append(buffer, ret);
	else
		this->request = buffer;

    // std::cout << "\n\n\n***Request***\n\n" << request << "\n\n***Request***\n\n\n" << std::endl;

    //if header is finished and not parsed
    if (IsHeaderFinished() && !IsHeaderParsed()) {

		// Parse the first line
		//---------------------------------------------------

		ParseFirstLine(request.substr(0, request.find("\r\n")));

		//---------------------------------------------------

        // Parse headers
		//---------------------------------------------------

		ParseHeaders(request.substr(request.find("\r\n") + 2, request.find("\r\n\r\n")));

		//---------------------------------------------------

        SetHeaderParsed(true);
		request = request.substr(request.find("\r\n\r\n") + 4);
    }
    
    //Use form data postman
    //if has body and header parsed and body not parsed yet
    if (IsHeaderParsed() && IsHasBody() && !IsBodyParsed())
    {
		
		BodyFd << request;

        if (GetHeadersValueOfKey("Transfer-Encoding") == "chunked")
		{
			//if file has 0/r/n/r/n
			//then strip all odd lines and /r/n
		}
		else 
		{
			//if file has /r/n/r/n
			//then strip all /r/n
		}

		//verfiy if body == content-length

		//IsBodyEqualContentLenght() ? SetBodyParsed(true) : (void)(StatusCode = 400);
    }
}