//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include <fstream>
#include "Cgi.h"

Cgi::Cgi(std::string const path, std::vector<std::string> ext) {
  std::ifstream file(path.c_str());
  if (!file.is_open()) {
      throw FileNotFoundException("cgi File not found");
  }
  file.close();
   if (ext.size() == 0)
       throw  IllegalArgumentException("extension not found");
    this->binaryPath = path;
}

Cgi::~Cgi() {

}
 bool Cgi::isCgi(std::string const path) const {
    std::string e = path.substr(path.find_last_of('.') + 1);
    if (std::find(this->ext.begin(), this->ext.end(), e) != this->ext.end())
        return true;
    return false;
}

std::string Cgi::getBinaryPath() {
    return this->binaryPath;
}



std::vector <std::string> Cgi::getExt() {
    return this->ext;
}

