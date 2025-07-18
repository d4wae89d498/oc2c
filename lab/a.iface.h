#ifndef ___a_h
# define ___a_h
# include <stdlib.h>

/****    IFACE START    *****/

typedef struct Root Root;
struct Root_vtable {
    size_t size;
    char *name;
    struct Root_vtable *super;
    Root*(*new)(Root*);
    void(*delete)(Root*);
    void(*sayHello)(Root*);
};
struct Root {
    struct Root_vtable* vtable;

};
extern Root * RootClass;
Root* __Root_new(Root * self);
void __Root_delete(Root * self);
void __Root_sayHello(Root * self);

/****    IFACE END    *****/

/****    IFACE START    *****/

typedef struct Name Name;
struct Name_vtable {
    size_t size;
    char *name;
    struct Root_vtable *super;
    Name*(*new)(Name*);
    void(*delete)(Name*);
    void(*sayHello)(Name*);
    Name*(*initWithValueand)(Name*, int, int);
    void(*printIvar)(Name*);
};
struct Name {
    struct Name_vtable* vtable;
    int     ivar;

};
extern Name * NameClass;
Name* __Name_new(Name * self);
void __Name_delete(Name * self);
void __Name_sayHello(Name * self);
Name* __Name_initWithValueand(Name * self, int value, int B);
void __Name_printIvar(Name * self);

/****    IFACE END    *****/

/****    IFACE START    *****/

typedef struct OtherName OtherName;
struct OtherName_vtable {
    size_t size;
    char *name;
    struct Root_vtable *super;
    OtherName*(*new)(OtherName*);
    void(*delete)(OtherName*);
    void(*sayHello)(OtherName*);
};
struct OtherName {
    struct OtherName_vtable* vtable;

};
extern OtherName * OtherNameClass;
OtherName* __OtherName_new(OtherName * self);
void __OtherName_delete(OtherName * self);
void __OtherName_sayHello(OtherName * self);

/****    IFACE END    *****/
#endif
