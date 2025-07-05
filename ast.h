#ifndef AST_H
#define AST_H

#include <stdlib.h>

// Forward declaration for ast_node_visitor
struct ast_node_visitor;
typedef struct ast_node_visitor ast_node_visitor;

typedef struct ast_node ast_node;
typedef struct ivar_node ivar_node;
typedef struct param_node param_node;
typedef struct method_node method_node;
typedef struct interface_node interface_node;
typedef struct implementation_node implementation_node;
typedef struct message_node message_node;
typedef struct selector_node selector_node;
typedef struct raw_node raw_node;
typedef struct tu_node tu_node;
typedef struct expr_node expr_node;

typedef struct ast_node_visitor 
{
    void *(*param_node)(param_node *self, void*);
    void *(*method_node)(method_node *self, void*);
    void *(*interface_node)(interface_node *self, void*);
    void *(*implementation_node)(implementation_node *self, void*);
    void *(*message_node)(message_node *self, void*);
    void *(*selector_node)(selector_node *self, void*);
    void *(*raw_node)(raw_node *self, void*);
    void *(*tu_node)(tu_node *self, void*);
    void *(*expr_node)(expr_node *self, void*);
} ast_node_visitor;

struct ast_node {
    void *(*accept)(ast_node *, ast_node_visitor, void*);
};


struct param_node {
    ast_node base;
    char *type;
    char *name;
};

struct method_node {
    ast_node base;
    int is_class_method; 
    char *return_type;
    char *selector;
    struct param_node **params;
    int param_count;
    char *body;
};

struct interface_node {
    ast_node base;
    char *name;
    char *superclass_name;
    char **ivars;
    int ivar_count;
    struct method_node **methods;
    int method_count;
};

struct implementation_node {
    ast_node base;
    char *name;
    char *superclass_name;
    struct method_node **methods;
    int method_count;
};

struct message_node {
    ast_node base;
    ast_node *receiver;
    char *selector;
    ast_node **args;
    int arg_count;
};

struct selector_node {
    ast_node    base;
    char        *name;
};

struct tu_node {
    ast_node base;

    ast_node    **childs;
    size_t      size;
};

struct raw_node
{
    ast_node base;

    char *source;
};

struct expr_node {
    ast_node base;
    ast_node **children;
    int child_count;
};

// _accept function prototypes
void *param_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *method_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *interface_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *implementation_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *message_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *selector_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *raw_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *tu_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);
void *expr_node_accept(ast_node *self, ast_node_visitor visitor, void* arg);

#endif // AST_H