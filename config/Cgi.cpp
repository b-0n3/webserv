//
// Created by Abdelouahad Ait hamd on 4/7/22.
//

#include <fstream>
#include "Cgi.h"



Cgi::Cgi(std::string  path, std::vector<std::string>  ext) {
  std::ifstream file(path.c_str(), std::ios::in |std::ios::app);
  if (!file.is_open()) {
      throw FileNotFoundException("cgi File not found");
  }
  file.close();
   if (ext.size() == 0)
       throw  IllegalArgumentException("extension not found");
    this->binaryPath = path;
    this->ext = ext;
}

Cgi::~Cgi() {

}
 bool Cgi::isCgi(std::string const path) const {
    std::string e = path.substr(path.find_last_of('.') );
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

Cgi *Cgi::fromNode(Node<Token *> *root) {
   std::string path = "";
   std::vector<std::string> ext;
    if (root->getChildren().size() != 2)
        throw IllegalArgumentException("cgi node must have 2 children");
    for (int i = 0; i < root->getChildren().size(); i++)
    {
        if (root->getChildren()[i]->getData()->getValue() == "path") {
            if (root->getChildren()[i]->getChildren().size() != 1)
                throw IllegalArgumentException("path node must have 1 child");
            path = root->getChildren()[i]->getChildren()[0]->getData()->getValue();
        }
       else if (root->getChildren()[i]->getData()->getValue() == "ext") {
            if (root->getChildren()[i]->getChildren().empty())
                throw IllegalArgumentException("ext node must have at least 1 child");
            for (int j = 0; j < root->getChildren()[i]->getChildren().size(); j++) {
                ext.push_back(root->getChildren()[i]->getChildren()[j]->getData()->getValue());
            }
        }
        else
            throw IllegalArgumentException("cgi node must have path and ext children");
    }
    if (path.empty())
        throw IllegalArgumentException("cgi node must have path child");
    if (ext.empty())
        throw IllegalArgumentException("cgi node must have ext child");
    return new Cgi(path, ext);
}

// @todo: write a function that will return a Cgi object from a node
void Cgi::execute(HttpRequest *pRequest, HttpResponse *pResponse) {
    std::string *env = new std::string[pRequest->GetHeaders().size()];
    for ()
}

