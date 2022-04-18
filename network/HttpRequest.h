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
#include <map>

class HttpRequest {
    private:
        int                                 Socketfd;   
        char                                Raw[5000];//Used char* for strtok compatibility    
        std::string                         Method;
        std::string                         Path;
        std::string                         Version;
        std::string                         Body;
        
        std::map<std::string, std::string>  Headers;
        std::map<std::string, std::string>  Params;
        
        bool                                HeaderParsed;
        bool                                BodyParsed;

        HttpRequest();

        void		SetHeaderParsed(bool parsed){HeaderParsed = parsed;}
        void		SetBodyParsed(bool parsed){BodyParsed = parsed;}
        void		SetMethod(int method){Method = method;}
        void		SetPath(std::string path){Path = path;}
        void		SetVersion(std::string version){Version = version;}
        void		SetHeaders(std::string key, std::string value){Headers.insert(std::pair<std::string, std::string>(key, value));}
        void		SetParams(std::string key, std::string value){Params.insert(std::pair<std::string, std::string>(key, value));}
        
        void        Parse();
        void        ContinueParse();
    public:
        HttpRequest(int fd);

        //Getters
        std::string GetMethod(){return Method;}
        std::string GetPath(){return Path;}
        std::string GetVersion(){return Version;}
        std::string GetBody(){return Body;}
        std::string GetHeadersValueOfKey(std::string key){return Headers.find(key)->second;}
        std::string GetParamsValueOfKey(std::string key){return Params.find(key)->second;}
        std::map<std::string, std::string> GetHeaders(){return Headers;}
        std::map<std::string, std::string> GetParams(){return Params;}

        // Utils
        bool        IsHeaderFinished();
        bool        IsHeaderParsed(){return HeaderParsed;}
        bool        IsBodyFinished();
        bool		IsBodyParsed(){return BodyParsed;}
        bool        IsFinished(){return IsHeaderFinished() && IsBodyFinished();}

        bool        IsHasBody(){return Method == "POST" ? true : false;}
};


#endif //WEBSERV_HTTPREQUEST_H
