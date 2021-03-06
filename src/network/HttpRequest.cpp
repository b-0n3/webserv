
#include <cstring>

#include "HttpRequest.h"
#include "StatusCode.h"
#include "../tools/Utils.h"
#include <sys/stat.h>
#include <sys/types.h>

HttpRequest::HttpRequest(int fd,char *address) : Socketfd(fd),
                                   request(""),
                                   buffer(new char[BUFFER_SIZE]),
                                   Method(""),
                                   Path(""),
                                   Version(""),
                                   StartedAt(time(NULL)),
                                   lastPacket(time(NULL)),
                                   HeaderParsed(false),
                                   BodyParsed(false),
                                   StatusCode(200) {

    Headers.clear(), Params.clear();
    this->setTimeOutAt(10L );
    this->startTimestamp = time(NULL);
    this->remoteAddress = address;
	srand(StartedAt);
    this->cgiRunning =false;
    this->bodyRemainingFromHeaders = "";
    std::string tmpDir = getenv("PWD");
    tmpDir += "/.tmp/";
	this->BodyFileName = tmpDir + std::to_string(rand());
	this->TmpBodyFileName = tmpDir + std::to_string(rand());
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
    Path = FirstLine.substr(FirstLine.find(' ') + 1);
    Path = Path.substr(0, Path.find(' '));


    if (Path.find("http://") != std::string::npos) {
        Path = Path.substr(7);

        SetHeaders("Host", Path.substr(0, Path.find("/")));
        Path = Path.substr(Path.find(" "));
    }
    

    if (Path.find("?") != std::string::npos){
		
        std::string params = Path.substr(Path.find("?") + 1);
        Path = Path.substr(0, Path.find("?"));
		std::stringstream ss(params);

        while (1)
        {
            std::string token;
			std::getline(ss, token, '&');
			if (token.empty())
				break;
            SetParams(token.substr(0, token.find("=")), token.substr(token.find("=") + 1));
        }
    }
    realPath = Path;
	Version = FirstLine.substr(FirstLine.find("HTTP/") + 5, FirstLine.find("\r\n"));
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
}

void    HttpRequest::Parse(  long long maxBodySize)
{
    int ret = 0;

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


    if (IsHeaderFinished() && !IsHeaderParsed()) {

       	ParseFirstLine(request.substr(0, request.find("\r\n")));


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

    }
    this->lastPacket = time(NULL);

     if (IsHeaderParsed() && IsHasBody() && !IsBodyParsed() ) {

        if (Headers.count("Content-Length") == 0) {
            SetBodyParsed(true);
            return;
        }
         long long contentLength = std::stoull(Headers["Content-Length"]);

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
                if (fileSize == (size_t)contentLength)
                    SetBodyParsed(true);
                if (fileSize > (size_t)contentLength) {
                    StatusCode = BAD_REQUEST;
                     SetBodyParsed(true);
                }
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
			if (fileSize == (size_t)contentLength)
				SetBodyParsed(true);
            if (fileSize > (size_t)contentLength) {
                StatusCode = BAD_REQUEST;
                SetBodyParsed(true);
             }
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

std::string HttpRequest::log() {
    std::stringstream  ss;
    char buff[20];
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&this->startTimestamp));
    std::string host = "";
    if (this->Headers.count("host") != 0)
        host = GetHeadersValueOfKey("host") + " ";
    ss << buff;
    ss << " ";
    ss << this->remoteAddress;
    ss << " ";
    ss << host;
    ss << this->GetMethod();
    ss << " ";
    ss << this->getRealPath();
    return ss.str();
}


bool HttpRequest::IsHasBody() { return Method == "POST" || Method == "DELETE"; }

bool HttpRequest::IsFinished() { return (IsHasBody() && IsBodyParsed()) || (!IsHasBody() && IsHeaderParsed()); }

bool HttpRequest::IsBodyParsed() { return BodyParsed; }

bool HttpRequest::IsHeaderParsed() { return HeaderParsed; }


HeadersMap &HttpRequest::GetHeaders(){ return Headers; }

std::map<std::string, std::string> &HttpRequest::GetParams() { return Params; }

int HttpRequest::getStatusCode() { return StatusCode; }


std::string &HttpRequest::GetMethod() { return Method; }
std::string &HttpRequest::GetPath(){return Path;}

std::string HttpRequest::GetVersion(){return Version;}
std::string &HttpRequest::getBodyFileName(){
    return this->BodyFileName;
}
int HttpRequest::GetBodyFd(){return open(BodyFileName.c_str(), O_RDONLY);}

std::string &HttpRequest::GetHeadersValueOfKey(std::string key){
    if (Headers.find(key) != Headers.end())
        return Headers[key];
    return Nothing(std::string);
}
std::string &HttpRequest::GetParamsValueOfKey(std::string key){
    if (Params.find(key) != Params.end())
        return Params[key];
    return Nothing(std::string); 
}

void  HttpRequest::SetPath(std::string path){Path = path;}


void HttpRequest::SetHeaderParsed(bool parsed) { HeaderParsed = parsed; }
void HttpRequest::SetBodyParsed(bool parsed) { BodyParsed = parsed; }
void HttpRequest::SetMethod(int method) { Method = method; }
void HttpRequest::SetVersion(std::string version) { Version = version; }

void HttpRequest::SetHeaders(std::string key, std::string value) {
    Headers.insert(std::pair<std::string, std::string>(key, value));
}
void HttpRequest::SetParams(std::string key, std::string value) {
    Params.insert(std::pair<std::string, std::string>(key, value));
}
