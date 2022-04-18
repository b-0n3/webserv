//
// Created by b0n3 on 4/17/22.
//

#include <unistd.h>
#include "HttpResponse.h"
HttpResponse::HttpResponse() {
    this->statusCode = 200;
    this->contentLength = 0;
    this->contentType = "text/html";
}
void HttpResponse::writeToFd(int i) {
    write(i, "dsfsddf", 8);
}
