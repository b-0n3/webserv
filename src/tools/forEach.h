//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#ifndef WEBSERV_FOREACH_H
#define WEBSERV_FOREACH_H
template <typename T>
struct forEach {
    T *data;
    struct forEach *args;
    forEach(T *data, struct forEach *args) : data(data), args(args) {}
    void apply(int i) {
        (*data)[i] = args->f(i, (*data)[i]);
    }
};
#endif //WEBSERV_FOREACH_H
