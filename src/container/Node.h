
#pragma once

#include <iostream>
#include <vector>

template<typename T>
class Node {
private:
    T data;
    Node<T> *parent;
    std::vector<Node<T> *> children;
public:
    Node() {
        parent = nullptr;
    }

    Node(T data) {
        this->data = data;
        parent = nullptr;
    }

    ~Node() {
        delete data;
    }

    Node(T data, Node<T> *parent) {
        this->data = data;
        this->parent = parent;
    }

    Node(T data, Node<T> *parent, std::vector<Node<T> *> children) {
        this->data = data;
        this->parent = parent;
        this->children = children;
    }

    T getData() {
        return data;
    }

    void setData(T data) {
        this->data = data;
    }

    Node<T> *getParent() {
        return parent;
    }

    void setParent(Node<T> *parent) {
        this->parent = parent;
    }

    std::vector<Node<T> *> getChildren() {
        return children;
    }

    bool addChild(Node<T> *child) {
        if (child == nullptr)
            return false;
        child->setParent(this);
        children.push_back(child);
        return true;
    }

    bool removeChild(Node<T> *child) {
        if (child == nullptr)
            return false;
        for (int i = 0; i < children.size(); i++) {
            if (children[i] == child) {
                children.erase(children.begin() + i);
                return true;
            }
        }
        return false;
    }

    void printNode(Node<T> *root) {
        if (root == nullptr)
            return;
        std::cout << "{" << *root->getData() << "}" << std::endl;
        std::cout << "{children}" << std::endl;
        for (int i = 0; i < root->getChildren().size(); i++) {
            printNode(root->getChildren()[i]);
        }
        std::cout << "{end of" << *root->getData() << std::endl;
    }

    bool operator==(Node<T> *node) {
        if (node == nullptr)
            return false;
        return data == node->getData();
    }
};