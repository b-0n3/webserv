//
// Created by Abdelouahad Ait hamd on 4/9/22.
//

#pragma once
#include "Node.h"
#include <vector>

template <typename T > class BinaryTree {
private:
    std::vector<Node<T>*> routes;
    int size;
public:
    BinaryTree() {
        size = 0;
    }
    ~BinaryTree() {
        for (int i = 0; i < size; i++) {
            delete routes[i];
        }
    }
    void addRoote(Node<T>* node) {
        routes.push_back(node);
        size++;
    }
    void remove(Node<T>* node) {
        for (int i = 0; i < routes.size(); i++) {
            if (routes[i] == node) {
                routes.erase(routes.begin() + i);
                size--;
                break;
            }
        }
    }
    Node<T>* get(int index) {
        return routes[index];
    }
    int getSize() {
        return size;
    }

    void deleteTree(Node<T> *root) {
        if (root == nullptr) {
            return;
        }
        for (int i = 0; i < root->getChildren().size(); i++) {
            deleteTree(root->getChildren()[i]);
        }
       // delete root;
    }
};
