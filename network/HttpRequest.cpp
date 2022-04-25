
#include <cstring>
#include <iostream>
#include "HttpRequest.h"

HttpRequest::HttpRequest(int fd) : Socketfd(fd),
                                   buffer("\0"),
                                   request(""),
                                   Method(""),
                                   Path(""),
                                   Version(""),
								   BodySize(0),                                   HeaderParsed(false),
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


// int hex(std::string ss)
// {
//     int n;
//     std::stringstream d("c");
//     std

//     d >> >> n;
//     return n;
// }

void    HttpRequest::Parse() {
    
    buffer[0] = '\0';
    int ret = 0;
    // To Check if this correct
    if ((ret = read(Socketfd, buffer, 5000-1)) < 0)
    {
        StatusCode = 400;
        return;
    }
		if(!IsHeaderParsed())
    		this->request.append(buffer, ret);
		else
		{
		    request = buffer;

			std::cout << request << std::endl;
			std::cout << __COUNTER__ << std::endl;
			std::cout << "DONE!!!" << std::endl;
			// exit(0);
		}


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
		
		std::cout << request << std::endl;
		checkBody = false;
		std::cout << GetHeadersValueOfKey("Content-Length") << std::endl;
    }

	std::cout << request << std::endl;
    if (IsHeaderParsed() && IsHasBody() && !IsBodyParsed())
    {
		//std::cout << request << std::endl;
        if (Headers.count("Transfer-Encoding") == 1) 
		{
			std::stringstream ss(request);
			std::string line;
			size_t hexa;
			int t = 0;
			while(1)
			{
				if(!checkBody)
				{
					std::getline(ss, line, '\n');
					std::cout << "line89:" << line << std::endl;
					if (line == "0\r")
					{
						std::cout << "line******:"  << std::endl;
						return;
					}
					std::stringstream d(line);
					d >> std::hex >> hexa;
					if (hexa == 0)
					{
						BodyFd.close();
						break;
					}
					std::string app(ss.str().substr(line.size() + 1, hexa ));
					BodyFd << app;
					if (app.size() < hexa)
					{
						checkBody = true;
						BodyRemaining = hexa - app.size();
						std::cout << "hexa:" << hexa << std::endl;
						std::cout<< "app:" << app.size() << std::endl;
						std::cout << "debug: " << BodyRemaining << std::endl;
						break;
						
					}
					else
					{
						request = request.c_str() + (line.size() + 1 + hexa + 2 ) ;
						//checkBody = false;
						BodyRemaining = 0;
						ss.clear();
						ss.str(request);
					}
					// std::cout << "app******:\n" << app << std::endl;
					// std::cout << "----" << std::endl;
					// std::cout << "Chunked1" << std::endl;

					
					// std::cout << "request******:" << request << std::endl;
					// std::cout << "Chunked2" << std::endl;
					// if (request.size() == 0)
					// 	break;
					
				}
				else
				{
					std::cout << "Chunked4" << std::endl;
					if (request.size() > BodyRemaining)
					{
						std::cout << "Chunked5" << std::endl;
						std::cout << "request.size(): " << request.size() << std::endl;
						std::cout << "BodyRemaining: " << BodyRemaining << std::endl;
						std::string app(request.substr(0, BodyRemaining));
						BodyFd << app;
						request = request.substr(BodyRemaining + 5);
						ss.clear();
						ss.str(request);
						std::cout << "line:" << line << std::endl;
						std::getline(ss, line, '\n');
						std::cout << "line:" << line << std::endl;
						std::cout << "*****request******:" << request << std::endl;
						BodyRemaining = 0;
						checkBody = false;
					}
					else
					{
						std::cout << "Chunked6" << std::endl;
						std::cout << "request.size(): " << request.size() << std::endl;
						std::cout << "BodyRemaining: " << BodyRemaining << std::endl;
						std::string app(request);
						BodyFd << app;
						request = "";
						
						BodyRemaining -= app.size();
						std::cout<< "debug6: " << BodyRemaining << std::endl;
						checkBody = true;
						break;
					}
					
				}
				//std::stringstream ss(request);
				//app = app.substr(0, hexa);
				//ss.str() += app.length() + 2;

				// if (checkBody)
				// {
				// 	line = line.substr(0, line.size());
				// 	BodyFd << line;
				// }
				// else
				// {
				// 	std::stringstream d(line);
				// 	d >> std::hex >> hexa;
				// 	std::cout << "hexa******:" << hexa << std::endl;
				// 	if (hexa == 0)
				// 	{
				// 		BodyFd.close();
				// 		std::cout << BodyFd << std::endl;
				// 		return;
				// 	}
				// 	checkBody = false;
				// }
				// while (1)
				// {
				// 	std::getline(ss, line, '\n');
				// 	std::cout << "body: " << line << std::endl;
				// 	//line = line.substr(0, line.size() - 1);
				// 	line += "\n";
		
				// 	//BodyFd << line;
				// 	//line = line.substr(0, line.size());
					
				// 	std::cout << "9abla+++++hex2: " << hexa << "linesize "<<line.size() <<std::endl;
				// 	hexa -= line.size()+1;
				// 	std::cout << "hex2: " << hexa << "linesize "<<line.size() <<std::endl;
				// 	if(hexa == 0)
				// 	{	
				// 		line = line.substr(0, line.size() - 1);
				// 		BodyFd << line;
				// 		break;
				// 	}
				// 	else if(hexa < 0)
				// 	{
				// 		BodyFd.close();
				// 		return;//error
				// 	}
				// 	BodyFd << line;
				// 	if (ss.eof())
				// 	{
				// 		checkBody = true;
				// 		return;
				// 	}
				// }

			}
		}
		else
		{
				BodySize += request.size();
				BodyFd << request;
				if (std::atoi(GetHeadersValueOfKey("Content-Length").c_str()) == BodySize)
				{
					SetBodyParsed(true);
					BodyFd.close();
				}
				else
					StatusCode = 400;
		}
		std::cout << "Chunked3" << std::endl;
    }
}