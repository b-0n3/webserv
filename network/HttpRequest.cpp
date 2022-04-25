
#include <cstring>
#include <iostream>
#include "HttpRequest.h"

HttpRequest::HttpRequest(int fd) : Socketfd(fd),
                                   buffer(new char[BUFFER_SIZE]),
                                   request(""),
                                   Method(""),
                                   Path(""),
                                   Version(""),
                                   HeaderParsed(false),
                                   BodyParsed(false),
                                   StatusCode(200) {

    Headers.clear(), Params.clear();
	
	srand(time(NULL));
	std::string filename1 = "/tmp/" + std::to_string(rand());
	srand(time(NULL));
	std::string filename2 = "/tmp/" + std::to_string(rand());

	std::cout << filename1 << std::endl;
	std::cout << filename2 << std::endl;

	PureBodyFd.open(filename1);
	CleanBodyFd.open(filename2);
    Parse();
}

bool HttpRequest::IsHeaderFinished()
{
    return ((request.find("\r\n\r\n") != std::string::npos) ? true : false) ;
}

bool HttpRequest::IsChunkedBodyFinished()
{
	//search for the end of the chunked body in PureBodyFd
	std::string line;
	while(PureBodyFd.good())
	{
		std::getline(PureBodyFd, line);
		if(line.equals("0\r\n"))
			return true;
	}
	return false;
}

//check if the condition is true !!!!!!!!!!!!!
bool HttpRequest::IsNormalBodyFinished()
{
	PureBodyFd.seekg(0, PureBodyFd.end);
	size_t length =PureBodyFd.tellg()
	PureBodyFd.seekg(0, PureBodyFd.beg);

	return (GetHeadersValueOfKey("Content-Length") != Headers.end() && std::stoi(GetHeadersValueOfKey("Content-Length")) == length);

}

void HttpRequest::ProcessChunkedBody()
{
	int i = 0;
	size_t line_size = 0;

	//strip the chunked body from PureBodyFd and put it in CleanBodyFd
	std::string line;
	while(PureBodyFd.good())
	{
		if( line == "0\r\n" )
			break;
		if (i % 2 == 0)
		{
			std::getline(PureBodyFd, line);
			std::stringstream ss(line);
			ss >> std::hex >> line_size; // E\r\n => 14
		}
		else
		{
			for (size_t i = 0; i < line_size; i++) // < 14
			{
				std::getline(PureBodyFd, line);
				CleanBodyFd << line;
				line_size -= line.length();
			}
		}
	}
}

void HttpRequest::ProcessNormalBody()
{

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

void    HttpRequest::Parse() {
    
    
    int ret = 0;
    // To Check if this correct
    if ((ret = read(Socketfd, buffer, BUFFER_SIZE - 1)) < 0)
    {
        StatusCode = 400;
        return;
    }
	
	buffer[ret] = '\0';

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

	std::cout << "Body: " << request << std::endl;
    
    //Use form data postman
    //if has body and header parsed and body not parsed yet
    if (IsHeaderParsed() && IsHasBody() && !IsBodyParsed())
    {
	// 	PureBodyFd << request;
        if (GetHeadersValueOfKey("Transfer-Encoding") != Headers.end() && GetHeadersValueOfKey("Transfer-Encoding") == "chunked")
		{
			std::cout << "chunked" << std::endl;
			if (IsChunkedBodyFinished())
			{
				ProcessChunkedBody();
				// IsBodyEqualContentLenght() ? SetBodyParsed(true) : (void)(StatusCode = 400);
			}
		}
		else 
		{
			std::cout << "Not chunked" << std::endl;
			if (IsNormalBodyFinished())
			{
				ProcessNormalBody();
				// IsBodyEqualContentLenght() ? SetBodyParsed(true) : (void)(StatusCode = 400);
			}
		}
    }
	std::cout << std::endl<< std::endl<< std::endl<< std::endl;
}