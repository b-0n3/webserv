//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#ifndef WEBSERV_FILTER_H
#define WEBSERV_FILTER_H

#include <vector>

template<typename T>
std::vector<T> filter(std::vector<T> &c, bool (*f)(T)) {
    std::vector<T> result;
    for (typename std::vector<T>::iterator it = c.begin(); it != c.end(); ++it) {
        if (f(*it)) {
            result.push_back(*it);
        }
    }
    return result;
}

#endif //WEBSERV_FILTER_H
