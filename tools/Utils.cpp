//
// Created by Abdelouahad Ait hamd on 4/13/22.
//

#include <sys/stat.h>
#include <fstream>
#include "Utils.h"
#include <sstream>
#include <dirent.h>

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
    return str.find_first_not_of("0123456789") == std::string::npos;
}

bool is_directory(const std::string &path) {
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
        return true;
    return false;
}

std::string getConentTypeFromFileName(std::string fileName) {
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
    if (contentTypes.find(fileName.substr(fileName.find_last_of(".") + 1)) != contentTypes.end()) {
        return contentTypes[fileName.substr(fileName.find_last_of(".") + 1)];
    }
    return "text/plain";
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

std::string readFileAndReturnString(std::string filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string autoIndexRead(std::string path)
{
    DIR *dir;
    struct dirent *diread;
    std::string content = "<html><body>";
    if ((dir = opendir(path.c_str())) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
            content.append("<a href=");
            content.append(diread->d_name);
            content.append("> ");
            content.append(diread->d_name);
            content.append("</a></br>");
        }
        closedir(dir);
        content.append("</pre></body></html>");
        return content;
    }
    else
        return "";
}