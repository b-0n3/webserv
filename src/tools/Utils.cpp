//
// Created by Abdelouahad Ait hamd on 4/13/22.
//

#include <sys/stat.h>
#include <fstream>
#include "Utils.h"
#include <sstream>
#include <dirent.h>
#include <unistd.h>
#include <cstring>

#define WHITESPACE " \t\n\r"
std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}


bool is_digits(const std::string &str) {
    return str.find_first_not_of("0123456789x") == std::string::npos;
}

bool is_directory(const std::string &path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;
}

std::string getContentTypeFromFileName(std::string fileName) {
    std::map<std::string, std::string> contentTypes;
    contentTypes["html"] = "text/html";
    contentTypes["css"] = "text/css";
    contentTypes["js"] = "application/javascript";
    contentTypes["jpg"] = "image/jpeg";
    contentTypes["jpeg"] = "image/jpeg";
    contentTypes["png"] = "image/png";
    contentTypes["gif"] = "image/gif";
    contentTypes["txt"] = "text/plain";
    contentTypes["pdf"] = "application/pdf";
    contentTypes["doc"] = "application/msword";
    contentTypes["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    contentTypes["xls"] = "application/vnd.ms-excel";
    contentTypes["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    contentTypes["ppt"] = "application/vnd.ms-powerpoint";
    contentTypes["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    contentTypes["mp3"] = "audio/mpeg";
    contentTypes["mp4"] = "video/mp4";
    contentTypes["m4a"] = "audio/mp4";
    contentTypes["m4v"] = "video/mp4";
    contentTypes["ogg"] = "audio/ogg";
    contentTypes["ogv"] = "video/ogg";
    contentTypes["wav"] = "audio/wav";
    contentTypes["webm"] = "video/webm";
    contentTypes["webp"] = "image/webp";
    contentTypes["ico"] = "image/x-icon";
    contentTypes["svg"] = "image/svg+xml";
    contentTypes["eot"] = "application/vnd.ms-fontobject";
    contentTypes["ttf"] = "application/x-font-ttf";
    contentTypes["woff"] = "application/x-font-woff";
    contentTypes["woff2"] = "application/x-font-woff2";
    contentTypes["otf"] = "application/x-font-otf";
    contentTypes["sfnt"] = "application/x-font-sfnt";
    contentTypes["json"] = "application/json";
    contentTypes["xml"] = "application/xml";
    contentTypes["zip"] = "application/zip";
    contentTypes["rar"] = "application/x-rar-compressed";
    contentTypes["7z"] = "application/x-7z-compressed";
    contentTypes["tar"] = "application/x-tar";
    contentTypes["gz"] = "application/x-gzip";
    contentTypes["bz2"] = "application/x-bzip2";
    if (contentTypes.find(fileName.substr(fileName.find_last_of(".") + 1)) != contentTypes.end()) {
        return contentTypes[fileName.substr(fileName.find_last_of(".") + 1)];
    }
    return "text/plain";
}

std::string getExtensionByContentType(std::string type)
{
    std::map<std::string , std::string > values;
    values[ "text/html"] = "html";
    values["text/css"] = "css" ;
    values["application/javascript"] = "js";
    values["image/jpeg"] = "jpg" ;
    values[ "image/jpeg"] = "jpeg";
    values["image/png"] = "png" ;
    values["image/gif"] = "gif" ;
    values["text/plain"] = "txt" ;
    values["application/pdf"] = "pdf" ;
    values["application/msword"] = "doc" ;
    values[ "application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = "docx";
    values["application/vnd.ms-excel"] = "xls" ;
    values[ "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = "xlsx";
    values["application/vnd.ms-powerpoint"] = "ppt" ;
    values[ "application/vnd.openxmlformats-officedocument.presentationml.presentation"] = "pptx";
    values["audio/mpeg"] = "mp3" ;
    values["video/mp4"] = "mp4" ;
    values["audio/mp4"] = "m4a" ;
    values["video/mp4"] = "m4v" ;
    values["audio/ogg"] = "ogg" ;
    values["video/ogg"] = "ogv" ;
    values["audio/wav"] = "wav" ;
    values[ "video/webm"] = "webm";
    values[ "image/webp"] = "webp";
    values["image/x-icon"] = "ico" ;
    values["image/svg+xml"] = "svg" ;
    values["application/vnd.ms-fontobject"] = "eot" ;
    values["application/x-font-ttf"] = "ttf" ;
    values[ "application/x-font-woff"] = "woff";
    values[ "application/x-font-woff2"] = "woff";
    values["application/x-font-otf"] = "otf" ;
    values[ "application/x-font-sfnt"] = "sfnt";
    values[ "application/json"] = "json";
    values["application/xml"] = "xml" ;
    values["application/zip"] = "zip" ;
    values["application/x-rar-compressed"] = "rar" ;
    values["application/x-7z-compressed"] = "7z";
    values["application/x-tar"] = "tar" ;
    values["application/x-gzip"] = "gz";
    values["application/x-bzip2"] = "bz2" ;
    if (values.find(type) != values.end()) {
        return "."+values[type];
    }
    return ".txt";
}

struct pollfd *convertToArray(std::vector< struct pollfd>  vec) {
    struct  pollfd *array = new struct pollfd[vec.size()];
    for (int i = 0; i < vec.size(); i++) {
        struct pollfd pfd;
        pfd.fd = vec[i].fd;
        pfd.events = vec[i].events;
        array[i] = pfd;
    }
    return array;
}
size_t countFileSize(const char *filename) {
    struct stat st;

    if (stat(filename, &st) == 0)
        return st.st_size ;
    return -1;
}

std::string toUpper(std::string &str)
{
    std::string result = str;
    for (int i = 0; i < result.size(); i++) {
        result[i] = toupper(result[i]);
    }
    return result;
}
std::string readFileAndReturnString(std::string filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool  autoIndexRead(int fd, std::string path)
{
    DIR *dir;
    struct dirent *diread;
        write(fd, "<html><body>", 12);
    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            write(fd,"<a href=", 8);
            write(fd,diread->d_name, strlen(diread->d_name));
            write(fd,"> ",2);
            write(fd,diread->d_name, strlen(diread->d_name));
            write(fd,"</a></br>",9);
        }
        closedir(dir);
        write(fd, "</pre></body></html>",20);
        close(fd);
        return true;
    }
    else
        return false;
}




std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}