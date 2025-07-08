#ifndef AST_H
#define AST_H

#include <stdlib.h>

#define make_ast(type, var, ...) \
    type *var = malloc(sizeof(type)); \
    *var = (type)__VA_ARGS__; \
    var->base.accept = type ## _accept; \

// Forward declaration for ast_visitor
struct ast_visitor;
typedef struct ast_visitor ast_visitor;

typedef struct ast ast;
typedef struct ivar ivar;
typedef struct param param;
typedef struct method method;
typedef struct interface interface;
typedef struct implementation implementation;
typedef struct message message;
typedef struct raw raw;
typedef struct tu tu;
typedef struct expr expr;

typedef struct ast_visitor 
{
    void *(*param)(param *self, void*);
    void *(*method)(method *self, void*);
    void *(*interface)(interface *self, void*);
    void *(*implementation)(implementation *self, void*);
    void *(*message)(message *self, void*);
    void *(*raw)(raw *self, void*);
    void *(*tu)(tu *self, void*);
    void *(*expr)(expr *self, void*);
} ast_visitor;

struct ast {
    void *(*accept)(ast *, ast_visitor, void*);
};


struct param {
    ast base;
    char *type;
    char *name; 
};

struct keyword_arg {
    char *keyword;
    struct param *param;
};

struct method {
    ast base;
    enum {static_method, member_method} method_type;
    char *return_type;    
    struct keyword_arg **keyword_args;
    size_t keyword_arg_count;
    ast *body;
};


struct interface {
    ast base;
    char *name;
    char *superclass_name;
    char **ivars;
    int ivar_count;
    struct method **methods;
    int method_count;
};

struct implementation {
    ast base;
    char *name;
    char *superclass_name;
    struct method **methods;
    int method_count;
};

struct message {
    ast base;
    ast *receiver;
    char *selector;
    ast **args;
    int arg_count;
};


struct raw {
    ast base;
    char *source;
};

struct expr {
    ast base;
    ast **children;
    int child_count;
};

struct binop_expr {
    ast base;
    ast *left;
    char *op;
    ast *right;
};

struct conditional_expr {
    ast base;
    ast *test;
    ast *consequent;
    ast *alternate;
};

struct unary_op_expr {
    ast base;
    char *op;
    ast *expr;
    enum {
        unary_op_expr_prefix,
        unary_op_expr_sufix
    } pos;
};

struct cast_expr {
    ast base;
    char *type;
    ast *expr;
};

struct call_expr {
    ast base;
    ast *callee;
    ast **args;
    size_t args_count;
};

struct member_access {
    ast base;
    ast *callee;
    char *id;
    enum {
        member_access_arrow,
        member_access_dot
    } pos;
};

struct tu {
    ast base;

    ast **childs;
    size_t size;
};

// _accept function prototypes
void *param_accept(ast *self, ast_visitor visitor, void* arg);
void *method_accept(ast *self, ast_visitor visitor, void* arg);
void *interface_accept(ast *self, ast_visitor visitor, void* arg);
void *implementation_accept(ast *self, ast_visitor visitor, void* arg);
void *message_accept(ast *self, ast_visitor visitor, void* arg);
void *raw_accept(ast *self, ast_visitor visitor, void* arg);
void *tu_accept(ast *self, ast_visitor visitor, void* arg);
void *expr_accept(ast *self, ast_visitor visitor, void* arg);

#endif // AST_H