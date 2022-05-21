//
// Created by Abdelouahad Ait hamd on 5/13/22.
//

#ifndef WEBSERV_TEMPFILE_H
#define WEBSERV_TEMPFILE_H
#include <stdlib.h>
#include <string>
#include <fcntl.h>
#include <unistd.h>

class TempFile {
private:
    std::string fileName;
    int fd;
    bool isMoved;
public:
    TempFile();
    ~TempFile();
    int _open();
    void _close();
    void setFd(int fd);
    int getFd();
   std::string &getFileName();
    bool transferTo(std::string path);
};


#endif //WEBSERV_TEMPFILE_H
