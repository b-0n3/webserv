//
// Created by Abdelouahad Ait hamd on 4/9/22.
//

#pragma once
#include <vector>
template <typename T>
class Node {
private:
    T data;
    Node<T> *parent;
    std::vector<Node<T> *> children;
public:
    Node(){
        parent = nullptr;
    }
    Node(T data){
        this->data = data;
        parent = nullptr;
    }
    Node(T data, Node<T> *parent){
        this->data = data;
        this->parent = parent;
    }
    Node(T data, Node<T> *parent, std::vector<Node<T> *> children){
        this->data = data;
        this->parent = parent;
        this->children = children;
    }
    T getData(){
        return data;
    }
    void setData(T data){
        this->data = data;
    }
    Node<T> *getParent(){
        return parent;
    }
    void setParent(Node<T> *parent){
        this->parent = parent;
    }
    std::vector<Node<T> *> getChildren(){
        return children;
    }
    bool addChild(Node<T> *child){
        if(child == nullptr)
            return false;
        child->setParent(this);
        children.push_back(child);
        return true;
    }
    bool removeChild(Node<T> *child){
        if(child == nullptr)
            return false;
        for(int i = 0; i < children.size(); i++){
            if(children[i] == child){
                children.erase(children.begin() + i);
                return true;
            }
        }
        return false;
    }
    bool operator==(Node<T> *node){
        if (node == nullptr)
            return false;
        return data == node->getData();
    }
};