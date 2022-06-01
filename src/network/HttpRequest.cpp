
#include <cstring>

#include "HttpRequest.h"
#include "StatusCode.h"
#include "../tools/Utils.h"

#include <sys/stat.h>
#include <sys/types.h>

HttpRequest::HttpRequest(int fd) : Socketfd(fd),
                                   buffer(new char[BUFFER_SIZE]),
                                   bodyRemainingFromHeaders(""),
                                   request(""),
                                   Method(""),
                                   Path(""),
                                   port(80),
                                   Version(""),
                                   StartedAt(time(NULL)),
                                   lastPacket(time(NULL)),
                                   cgiRunning(false),
                                   HeaderParsed(false),
                                   BodyParsed(false),
                                   StatusCode(200) {

    Headers.clear(), Params.clear();
    this->setTimeOutAt(10L );
	srand(StartedAt);
    std::string tmpDir = getenv("PWD");
    tmpDir += "/.tmp/";
	this->BodyFileName = tmpDir + std::to_string(rand());
	this->TmpBodyFileName = tmpDir + std::to_string(rand());

//	std::cout << "body" << BodyFileName << std::endl;
//	std::cout << "tmp" << TmpBodyFileName << std::endl;
   // Parse(-1);
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

    if (Headers.count("Host") ==  0)
    {
        StatusCode = 400;
        SetHeaders("HOST", "");
        return;
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
    if ((ret = read(Socketfd, buffer, BUFFER_SIZE - 1)) < 0  || this->isTimeOut())
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
		request  = std::string(buffer, ret);
    std::cout << "request: " << request << std::endl;
    //if header is finished and not parsed
    if (IsHeaderFinished() && !IsHeaderParsed()) {
		// Parse the first line
        std::cout << "First Line: " << " somthing " << std::endl;
		ParseFirstLine(request.substr(0, request.find("\r\n")));

        // Parse headers
		ParseHeaders(request.substr(request.find("\r\n") + 2, request.find("\r\n\r\n")));

        SetHeaderParsed(true);
        unsigned long long contentLength = 0;
        if(Headers.count("Content-Length") !=  0)
            contentLength = std::stoull(GetHeadersValueOfKey("Content-Length"));
		this->bodyRemainingFromHeaders = request.substr(request.find("\r\n\r\n") + 4);
        if (this->bodyRemainingFromHeaders.size() < contentLength)
            return ;
        else
            request = std::string();
        // std::cout << "request: " << request << std::endl;
    }
    this->lastPacket = time(NULL);
    //Use form data postman
    //if has body and header parsed and body not parsed yet
    std::cout << "bodyRemainingFromHeaders: " << bodyRemainingFromHeaders << std::endl;
    if (IsHeaderParsed() && IsHasBody() && !IsBodyParsed() ) {
		//change compare of the map
        if (Headers.count("Content-Length") == 0) {
            SetBodyParsed(true);
            return;
        }
        unsigned long long contentLength = std::stoull(Headers["Content-Length"]);

        if (contentLength > maxBodySize && maxBodySize != -1) {
            StatusCode = MAX_BODY_SIZE_EXCEEDED;
            SetBodyParsed(true);
            return;
        }
        size_t fileSize;
        if (Headers.count("Transfer-Encoding") !=  0 && GetHeadersValueOfKey("Transfer-Encoding") == "chunked")
		{

			TmpBodyFd.open(TmpBodyFileName,  std::fstream::in | std::fstream::out | std::fstream::app | std::ios::binary ) ;
            if (!bodyRemainingFromHeaders.empty())
            {
                TmpBodyFd.write(bodyRemainingFromHeaders.c_str(), bodyRemainingFromHeaders.size());
                bodyRemainingFromHeaders.clear();
            }
			TmpBodyFd.write(request.c_str() , request.size());
             fileSize = CountFileSize(BodyFileName.c_str());
			if (IsChunkedBodyFinished()) {
                ProcessChunkedBody();
                if (fileSize == contentLength)
                    SetBodyParsed(true);
                if (fileSize > contentLength) {
                    StatusCode = BAD_REQUEST;
                    std::cout << fileSize << " " << contentLength << std::endl;
                    SetBodyParsed(true);
                }
              //  std::cout << CountFileSize(BodyFileName.c_str())  << "contentLenght " << contentLength<< std::endl;
			}
			TmpBodyFd.close();
		}
		else
		{
			BodyFd.open(BodyFileName,  std::fstream::in | std::fstream::out | std::fstream::app | std::ios::binary);
            if (!bodyRemainingFromHeaders.empty())
            {
                BodyFd.write(bodyRemainingFromHeaders.c_str(), bodyRemainingFromHeaders.size());
                bodyRemainingFromHeaders.clear();
            }
			BodyFd.write(request.c_str() , request.size());
			BodyFd.close();
             fileSize = CountFileSize(BodyFileName.c_str());
			if (fileSize == contentLength)
				SetBodyParsed(true);
            if (fileSize > contentLength) {
                StatusCode = BAD_REQUEST;
                SetBodyParsed(true);
                std::cout << fileSize << " " << contentLength << std::endl;
            }
             //   std::cout << CountFileSize(BodyFileName.c_str())  << "contentLenght unchenked " << contentLength<< std::endl;
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

void HttpRequest::setTimeOutAt(long timeOutAt) {
    this->timeOutAt = lastPacket + (timeOutAt);
}

bool HttpRequest::isTimeOut() {

    return timeOutAt < time(NULL);
}

long HttpRequest::getLastPacket() const {
    return lastPacket;
}

void HttpRequest::setLastPacket(long lastPacket) {
    HttpRequest::lastPacket = lastPacket;
}
