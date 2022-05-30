
#include <cstring>

#include "HttpRequest.h"
#include "StatusCode.h"
#include "../tools/Utils.h"

#include <sys/stat.h>
#include <sys/types.h>

HttpRequest::HttpRequest(int fd) : Socketfd(fd),
                                   buffer(new char[BUFFER_SIZE]),
                                   request(""),
                                   Method(""),
                                   Path(""),
                                   port(80),
                                   Version(""),
                                   StartedAt(time(NULL)),
                                   cgiRunning(false),
                                   HeaderParsed(false),
                                   BodyParsed(false),
                                   StatusCode(200) {

    Headers.clear(), Params.clear();
    this->setTimeOutAt(20L );
	srand(StartedAt);
    std::string tmpDir = getenv("PWD");
    tmpDir += "/.tmp/";
	this->BodyFileName = tmpDir + std::to_string(rand());
	this->TmpBodyFileName = tmpDir + std::to_string(rand());

//	std::cout << "body" << BodyFileName << std::endl;
//	std::cout << "tmp" << TmpBodyFileName << std::endl;
    Parse(0);
}

size_t CountFileSize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;
    return -1; 
}

size_t HexadecimaltoDecimal(std::string str)
{
	size_t n;
    std::stringstream d(str);
    d >> std::hex >> n;
	return n;
}

bool HttpRequest::IsHeaderFinished()
{
    return ((request.find("\r\n\r\n") != std::string::npos) ? true : false) ;
}

bool HttpRequest::IsChunkedBodyFinished()
{
	bool ret  = false;
	const int X = 5;
	char* buf = new char[X];
	TmpBodyFd.seekg(-X, std::ios::end);
    TmpBodyFd.read(buf, X);
	if (strcmp("0\r\n\r\n", buf) == 0)
		ret = true;
	delete [] buf;
	return ret;
}

size_t  HttpRequest::ParseHexaLine()
{
	char buffer[256];
	char c;
	int count = 0;
	while (1)
	{
		TmpBodyFd.read(&c, 1);
		std::cout << c << std::endl;
		if (c == '\r')
			break;
		buffer[count++] = c;
 	}
	buffer[count] = 0;
	TmpBodyFd.read(&c, 1);
	return HexadecimaltoDecimal(std::string(buffer));
}

void HttpRequest::ProcessChunkedBody()
{
	BodyFd.open(BodyFileName,  std::fstream::in | std::fstream::out | std::fstream::app | std::ios::binary);
	TmpBodyFd.seekg(std::ios_base::beg);
	size_t ret = ParseHexaLine();
	char c[2];
	while(ret != 0)
	{
		char *buffer = new char[ret];
		TmpBodyFd.read(buffer, ret);
		TmpBodyFd.read(c, 2);
		BodyFd.write(buffer, ret);
		ret = ParseHexaLine();
	}
	BodyFd.close();
}

void  HttpRequest::ParseFirstLine(std::string FirstLine)
{
	Method = FirstLine.substr(0, FirstLine.find(' '));
    std::cout << "method =- "  << Method << std::endl;
    Path = FirstLine.substr(FirstLine.find(' ') + 1);
    Path = Path.substr(0, Path.find(' '));

	//std::cout << "Method: " << Method << std::endl;
    //std::cout << FirstLine<<std::endl;
	//Path = FirstLine.substr(FirstLine.find(' '), FirstLine.find("HTTP/") - 5 );
	std::cout << "Path: " << Path << std::endl;

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
    realPath = Path;
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
		token.erase(token.find("\r") , 1);
		SetHeaders(token.substr(0, token.find(":")), token.substr(token.find(":") + 2));
	}

    std::string host = GetHeadersValueOfKey("Host");
    if (host.find_first_of(':') != std::string::npos) {
        this->port = std::atoi(host.substr(host.find_first_of(':') + 1).c_str());
        host = host.substr(0, host.find_first_of(':'));
    }
    this->Headers["Host"] = host;
	// std::cout << "\rfff" << std::endl;
	// std::map<std::string, std::string> headersss = GetHeaders();
	// for (std::map<std::string, std::string>::iterator it = headersss.begin(); it != headersss.end(); ++it)
	// 	std::cout << it->first << " => " << it->second << std::endl;
	// 	std::cout << headersss["Transfer-Encoding"] << "====" << std::endl;
	// 		print_memory(headersss["Transfer-Encoding"]);

}

void    HttpRequest::Parse(unsigned  long long maxBodySize)
{
    
    int ret = 0;
    // To Check if this correct
    if ((ret = read(Socketfd, buffer, BUFFER_SIZE - 1)) < 0 )
    {
        StatusCode = TIMEOUT;
        SetBodyParsed(true);
        SetHeaderParsed(true);
        return;
    }
	buffer[ret] = '\0';

    if (!IsHeaderParsed())
    	this->request.append(buffer, ret);
	else
		this->request  = std::string(buffer, ret);

    //if header is finished and not parsed
    if (IsHeaderFinished() && !IsHeaderParsed()) {
		// Parse the first line
		ParseFirstLine(request.substr(0, request.find("\r\n")));

        // Parse headers
		ParseHeaders(request.substr(request.find("\r\n") + 2, request.find("\r\n\r\n")));

        SetHeaderParsed(true);
		request = request.substr(request.find("\r\n\r\n") + 4);

        return;
    }

    //Use form data postman
    //if has body and header parsed and body not parsed yet
    if (IsHeaderParsed() && IsHasBody() && !IsBodyParsed() ) {
		//change compare of the map
        if (Headers.count("Content-Length") == 0) {
            SetBodyParsed(true);
            return;
        }
        unsigned long long contentLength = std::stoull(Headers["Content-Length"]);
//        std::cout << "Content-Length: " << contentLength << std::endl;
//        std::cout << "max body size: " << maxBodySize << std::endl;

        if (contentLength > maxBodySize) {
            StatusCode = MAX_BODY_SIZE_EXCEEDED;
            SetBodyParsed(true);
            return;
        }

        if (Headers.count("Transfer-Encoding") !=  0 && GetHeadersValueOfKey("Transfer-Encoding") == "chunked")
		{
			TmpBodyFd.open(TmpBodyFileName,  std::fstream::in | std::fstream::out | std::fstream::app | std::ios::binary ) ;
            //std::cout << request<< std::endl;
			TmpBodyFd.write(request.c_str() , request.size());
			if (IsChunkedBodyFinished())
			{
				ProcessChunkedBody();
				if (CountFileSize(BodyFileName.c_str()) >=contentLength)
					SetBodyParsed(true);
                std::cout << CountFileSize(BodyFileName.c_str()) << std::endl;
			}
			TmpBodyFd.close();
		}
		else
		{
			BodyFd.open(BodyFileName,  std::fstream::in | std::fstream::out | std::fstream::app | std::ios::binary);
			BodyFd.write(request.c_str() , request.size());
			BodyFd.close();
			if (CountFileSize(BodyFileName.c_str()) >= contentLength)
				SetBodyParsed(true);
            std::cout << CountFileSize(BodyFileName.c_str()) << std::endl;
		}
    }
}

const std::string &HttpRequest::getRealPath() const {
    return realPath;
}

void HttpRequest::setRealPath(const std::string &realPath) {
    HttpRequest::realPath = realPath;
}

HttpRequest::~HttpRequest() {
    std::cout << "HttpRequest Destructor" << std::endl;
    if (TmpBodyFd.is_open())
        TmpBodyFd.close();
    if (BodyFd.is_open())
        BodyFd.close();
   int fd = open(TmpBodyFileName.c_str(), O_RDONLY);
   if (fd != -1)
   {
       close(fd);
       remove(TmpBodyFileName.c_str());
   }
   fd = open(BodyFileName.c_str(), O_RDONLY);
   if (fd != -1)
   {
       close(fd);
       remove(BodyFileName.c_str());
   }


        delete [] buffer;
}

void HttpRequest::setTimeOutAt( long timeOutAt) {
    this->timeOutAt = StartedAt + (timeOutAt);
}

bool HttpRequest::isTimeOut() {

    return timeOutAt < time(NULL);
}
