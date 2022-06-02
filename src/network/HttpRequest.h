//
//
// Created by b0n3 on 4/17/22.
//
//
// Edited by Za7aDotexe on 4/18/22.
//

#ifndef WEBSERV_HTTPREQUEST_H
#define WEBSERV_HTTPREQUEST_H
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "StatusCode.h"
#include <fcntl.h>
#include "../tools/Utils.h"

#define BUFFER_SIZE 50000
#define TIMEOUT 60

#define Nothing(Type) *(Type*)nullptr

typedef std::map<std::string, std::string, compareStringIgnoreCase> HeadersMap;

class HttpRequest {
    private:
        int				Socketfd;
        std::string 	request;
        char 			*buffer;
        std::string 	Method;
        std::string 	Path;
        std::string 	Version;
        std::string		BodyFileName;
        std::string		TmpBodyFileName;
        std::fstream 	BodyFd;
        std::fstream 	TmpBodyFd;
        time_t          StartedAt;
        std::string     realPath;
        long            timeOutAt;
        time_t          startTimestamp;
        long            lastPacket;
        
        std::map<std::string, std::string> Params;

        bool HeaderParsed;
        bool BodyParsed;

        int StatusCode;

        HttpRequest();

        void SetHeaderParsed(bool parsed);
        void SetBodyParsed(bool parsed);
        void SetMethod(int method);
        void SetVersion(std::string version);

        void SetHeaders(std::string key, std::string value);
        void SetParams(std::string key, std::string value);
    public:
	    std::string root;
        std::string location ;
        int  port;
        std::string remoteAddress;
        pid_t cgiPid;
        bool cgiRunning;
        std::string bodyRemainingFromHeaders;
        long getLastPacket() const;
        void setLastPacket(long lastPacket);
        std::string log();
	    HttpRequest(int fd, char *address);
        ~HttpRequest();
    
        void                setRealPath(const std::string &realPath);
        void                SetPath(std::string path);

  
	    HeadersMap Headers;

        void Parse( long long maxBodySize);
        void setTimeOutAt( long timeOutAt);

  	//Getters
    const std::string   &getRealPath() const;
    std::string &GetMethod();
    std::string &GetPath();
    
    std::string GetVersion();
    std::string &getBodyFileName();
    int GetBodyFd();
    
    std::string &GetHeadersValueOfKey(std::string key);
    std::string &GetParamsValueOfKey(std::string key);

    HeadersMap &GetHeaders();
    std::map<std::string, std::string> &GetParams();
    int getStatusCode();

    // Utils
	void ParseFirstLine(std::string);
	void ParseHeaders( std::string );
    bool IsHeaderFinished();
    bool IsHeaderParsed();
    bool IsBodyParsed();
    bool IsFinished();
    bool IsHasBody();
	bool IsChunkedBodyFinished();
	void ProcessChunkedBody();
	size_t  ParseHexaLine();
    bool isTimeOut();
};

#endif //WEBSERV_HTTPREQUEST_H
