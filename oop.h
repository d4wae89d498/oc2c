#ifndef OOP_H
# define OOP_H

#define construct(type, var, ...) \
    type *var = malloc(sizeof(type)); \
    *var = (type)__VA_ARGS__; \
    var->base.accept = type ## _accept; \

#endif