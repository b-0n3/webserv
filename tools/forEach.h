//
// Created by Abdelouahad Ait hamd on 4/8/22.
//

#ifndef WEBSERV_FOREACH_H
#define WEBSERV_FOREACH_H
template <typename T>
void forEach(T *array, int size, void (*f)(T)) {
    for (int i = 0; i < size; i++) {
        f(array[i]);
    }
}
#endif //WEBSERV_FOREACH_H
