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

};
