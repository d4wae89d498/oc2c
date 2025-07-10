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
typedef struct method method;
typedef struct interface interface;
typedef struct implementation implementation;
typedef struct message message;
typedef struct raw raw;
typedef struct top_level top_level;
typedef struct expr expr;
typedef struct binop_expr binop_expr;
typedef struct conditional_expr conditional_expr;
typedef struct unary_op_expr unary_op_expr;
typedef struct cast_expr cast_expr;
typedef struct keyword_arg keyword_arg;
typedef struct compound_statement compound_statement;
typedef struct statement statement;
typedef struct message_param message_param;
typedef struct selector selector;
typedef struct encode encode;

typedef struct ast_visitor 
{
    void *(*method)(method *self, void*);
    void *(*interface)(interface *self, void*);
    void *(*implementation)(implementation *self, void*);
    void *(*message)(message *self, void*);
    void *(*raw)(raw *self, void*);
    void *(*top_level)(top_level *self, void*);
    void *(*expr)(expr *self, void*);
    void *(*statement)(statement *self, void*);
    void *(*message_param)(message_param *self, void*);
    void *(*selector)(selector *self, void*);
    void *(*encode)(encode *self, void*);
    void *(*binop_expr)(binop_expr *self, void*);
    void *(*compound_statement)(compound_statement *self, void*);
    void *(*unary_op_expr)(unary_op_expr *self, void*);
    void *(*cast_expr)(cast_expr *self, void*);
    void *(*conditional_expr)(conditional_expr *self, void*);
    void *(*keyword_arg)(keyword_arg *self, void*);
} ast_visitor;

struct ast {
    void *(*accept)(ast *, ast_visitor, void*);
};


///////////////////////////////////

struct message_param {
    ast base;

    char *keyword;
    expr *value;
};

struct keyword_arg {
    ast base;
    char *keyword;
    char *type;
    char *name; 
};

struct method {
    ast base;
    enum {static_method, member_method} method_type;
    char *return_type;    
    keyword_arg **keyword_args;
    size_t keyword_arg_count;
    ast *body;
};


struct interface {
    ast base;
    char *name;
    char *superclass_name;
    compound_statement *ivars;
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

///////////////////////////////////

struct message {
    ast base;
    expr *receiver;
    message_param **params;
    int params_count;
};


struct expr {
    ast base;
    ast **exprs;
    int exprs_count;
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
    ast* arg;
};

struct cast_expr {
    ast base;
    char *type;
    ast *expr;
};

struct raw {
    ast base;
    char *source;
};

struct selector {
    ast base;
    char *str;
};

struct encode {
    ast base;
    char *type;
};


struct statement {
    ast base;
    enum {
        is_raw, 
        is_expr,
        is_cp
    } statement_type;
    union {
        raw *r_val;
        expr *e_val;
        compound_statement *cp_val;
    };
};

struct top_level {
    ast base;

    ast **childs;
    size_t size;
};

struct compound_statement {
    ast base;

    statement **statements;
    size_t statements_count;
};
// _accept function prototypes
void *statement_accept(ast *self, ast_visitor visitor, void *arg);
void *compound_statement_accept(ast *self, ast_visitor visitor, void* arg);
void *method_accept(ast *self, ast_visitor visitor, void* arg);
void *interface_accept(ast *self, ast_visitor visitor, void* arg);
void *implementation_accept(ast *self, ast_visitor visitor, void* arg);
void *message_accept(ast *self, ast_visitor visitor, void* arg);
void *raw_accept(ast *self, ast_visitor visitor, void* arg);
void *top_level_accept(ast *self, ast_visitor visitor, void* arg);
void *expr_accept(ast *self, ast_visitor visitor, void* arg);
void *binop_expr_accept(ast *self, ast_visitor visitor, void* arg);
void *conditional_expr_accept(ast *self, ast_visitor visitor, void* arg);
void *unary_op_expr_accept(ast *self, ast_visitor visitor, void* arg);
void *cast_expr_accept(ast *self, ast_visitor visitor, void* arg);
void *keyword_arg_accept(ast *self, ast_visitor visitor, void* arg);
void *message_param_accept(ast *self, ast_visitor visitor, void* arg);
void *selector_accept(ast *self, ast_visitor visitor, void* arg);
void *encode_accept(ast *self, ast_visitor visitor, void* arg);


#endif // AST_H