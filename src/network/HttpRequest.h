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

#include <fcntl.h>

#define BUFFER_SIZE 5001

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
    
	std::map<std::string, std::string> Headers;
	HttpRequest(int fd);
    void Parse();
	
  	//Getters
    std::string GetMethod() { return Method; }
    std::string GetPath(){return Path;}
    void SetPath(std::string path){Path = path;}
    std::string GetVersion(){return Version;}
    int GetBodyFd(){return open(BodyFileName.c_str(), O_RDONLY);}
    std::string GetHeadersValueOfKey(std::string key){return Headers.find(key)->second;}
    std::string GetParamsValueOfKey(std::string key){return Params.find(key)->second;}
    std::map<std::string, std::string> GetHeaders(){ return Headers; }
    std::map<std::string, std::string> GetParams() { return Params; }
    int getStatusCode() { return StatusCode; }

    // Utils
	void ParseFirstLine( std::string );

	void ParseHeaders( std::string );

    bool IsHeaderFinished();

    bool IsHeaderParsed() { return HeaderParsed; }


    bool IsBodyParsed() { return BodyParsed; }

    bool IsFinished() { return (IsHeaderFinished() && IsBodyParsed()) || (!IsHasBody() && IsHeaderParsed()); }

    bool IsHasBody() { return Method == "POST"; }


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
