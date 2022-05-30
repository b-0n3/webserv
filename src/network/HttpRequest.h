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

#define BUFFER_SIZE 20000
#define TIMEOUT 60

class HttpRequest {
private:
    int				Socketfd;
    std::string 	request;
    char 			*buffer;//Used char* for strtok compatibility
    std::string 	Method;
    std::string 	Path;
    std::string 	Version;
	std::string		BodyFileName;
	std::string		TmpBodyFileName;
    // @Todo change to ../tools tempFile.cpp
    std::fstream 	BodyFd;
    std::fstream 	TmpBodyFd;
    time_t          StartedAt;
    std::string     realPath;
      long       timeOutAt;
      long startTimestamp;
    // change this
    std::map<std::string, std::string> Params;

    bool HeaderParsed;
    bool BodyParsed;

    int StatusCode;

    HttpRequest();

    void SetHeaderParsed(bool parsed) { HeaderParsed = parsed; }

    void SetBodyParsed(bool parsed) { BodyParsed = parsed; }

    void SetMethod(int method) { Method = method; }
    void SetVersion(std::string version) { Version = version; }

    void SetHeaders(std::string key, std::string value) {
        Headers.insert(std::pair<std::string, std::string>(key, value));
    }
    void SetParams(std::string key, std::string value) {
        Params.insert(std::pair<std::string, std::string>(key, value));
    }
public:
    const std::string &getRealPath() const;
    ~HttpRequest();
    void setRealPath(const std::string &realPath);
    
	std::map<std::string, std::string, compareStringIgnoreCase> Headers;
	HttpRequest(int fd);
    void Parse(unsigned  long long maxBodySize);
    void setTimeOutAt( long timeOutAt);
    bool isTimeOut();
  	//Getters
    std::string GetMethod() { return Method; }
    std::string GetPath(){return Path;}
    void SetPath(std::string path){Path = path;}
    std::string GetVersion(){return Version;}
    std::string &getBodyFileName(){
        return this->BodyFileName;
    }
    int GetBodyFd(){return open(BodyFileName.c_str(), O_RDONLY);}
    std::string GetHeadersValueOfKey(std::string key){
        if (Headers.find(key) != Headers.end())
            return Headers[key];
        return std::string ();
    }
    std::string GetParamsValueOfKey(std::string key){
        if (Params.find(key) != Params.end())
            return Params[key];
        return std::string ();
    }
    std::map<std::string, std::string,compareStringIgnoreCase> GetHeaders(){ return Headers; }
    std::map<std::string, std::string> GetParams() { return Params; }
    int getStatusCode() { return StatusCode; }

    // Utils
	void ParseFirstLine( std::string );

	void ParseHeaders( std::string );

    bool IsHeaderFinished();

    bool IsHeaderParsed() { return HeaderParsed; }


    bool IsBodyParsed() { return BodyParsed; }

    bool IsFinished() { return (IsHasBody() && IsBodyParsed()) || (!IsHasBody() && IsHeaderParsed()); }

    bool IsHasBody() { return Method == "POST" || Method == "DELETE"; }


	bool IsChunkedBodyFinished();
	void ProcessChunkedBody();
	size_t  ParseHexaLine();



	/////////////////
	std::string root;
    std::string location ;
    int  port;
    std::string remoteAddress;
    pid_t cgiPid;
    bool cgiRunning;
	/////////////////

};

#endif //WEBSERV_HTTPREQUEST_H
