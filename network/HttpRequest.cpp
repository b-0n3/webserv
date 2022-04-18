
#include "HttpRequest.h"

HttpRequest::HttpRequest(int fd) :  Socketfd(fd),
                                    Raw(""),
                                    Method(""),
                                    Path(""),
                                    Version(""),
                                    Body(""),
                                    HeaderParsed(false),
                                    BodyParsed(false)
{
    Headers.clear(), Params.clear();
    Parse();
}

bool HTTPRequest::IsHeaderFinished()
{
    int i = 0;
    while (Raw[i] != '\0')
    {
        if (Raw[i] == '\r' && Raw[i + 1] == '\n' && Raw[i + 2] == '\r' && Raw[i + 3] == '\n')
            return true;
        i++;
    }
    return false;
}

void HttpRequest::ContinueParse()
{
    //Read And Concatinate The Raw Buffer
}

void    HttpRequest::Parse()
{
    // To Check if this correct
    if (read(Socketfd, Raw, 5000) < 0)
        throw std::runtime_error("Error while reading from socket");
    
    //if header is finished and not parsed
    if (IsHeaderFinished() && !IsHeaderParsed())
    {
        //parse header
        Method = strtok(Raw, " ");
        Path = strtok(NULL, " ");
        strtok(NULL, "/"); //skip "HTTP/""
        Version = strtok(NULL, "\r\n");

        //parse headers
        while (1)
        {
            std::string token =  strtok(NULL, "\r\n");
            if (token[0] == '\r' || token[1] == '\n')
                break;
            //Need Syntax check !!
            SetHeaders(token.substr(0, token.find(":")), token.substr(token.find(":") + 2));
        }
        SetHeaderParsed(true);
    }
    //if has body and not parsed
    if (IsHasBody() && !IsBodyParsed())
    {
        //parse body
        SetBodyParsed(true);
    }
}


