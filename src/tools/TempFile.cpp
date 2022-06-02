//
// Created by Abdelouahad Ait hamd on 5/13/22.
//



#include "TempFile.h"

TempFile::TempFile() {

    char tmp[] = ".tmp/uniqueXXXXXX";
    this->fd = mkstemp(tmp);
    fileName = tmp;
    isMoved = false;
}

TempFile::~TempFile() {
    close(this->fd);
}

void TempFile::_close() {
    close(this->fd);
}

int TempFile::_open() {
    this->fd = open(fileName.c_str(), O_RDWR);
    return this->fd;
}

std::string &TempFile::getFileName() {
    return this->fileName;
}

bool TempFile::transferTo(std::string path) {
    if (rename(fileName.c_str(), path.c_str()) == 0) {
        isMoved = true;
        this->fileName = path;
        return true;
    }
    return false;
}

int TempFile::getFd() {
    return this->fd;
}

void TempFile::setFd(int fd) {
    close(this->fd);
    this->fd = fd;
}

void TempFile::deleteFile() {
    close(this->fd);
    if (!isMoved)
        remove(fileName.c_str());
}

