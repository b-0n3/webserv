
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

void    HttpRequest::Parse()
{
    
}


