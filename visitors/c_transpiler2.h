#ifndef C_TRANSPILER2_VISITOR_H
#define C_TRANSPILER2_VISITOR_H

#include "../ast.h"
#include <stddef.h>
#include <stdio.h>

extern ast_visitor c_transpiler2_visitor;

typedef struct class_map
{
    struct  class_map *parent;
    char    *name;
    method  *methods[255];
    size_t  methods_count;
    method  *instancetypes[255];
    size_t  instancetypes_count;
} class_map;

typedef struct c_transpiler2_ctx 
{
    FILE    *iface;
    FILE    *impl;
    FILE    *init;

    FILE    *current;
    char    *current_iface;

    class_map *classes[255];



    size_t  classes_count;
} c_transpiler2_ctx;

c_transpiler2_ctx    c_transpiler2_ctx_init();
void                c_transpiler2_ctx_dump(c_transpiler2_ctx *ctx);

void *compound_statement_to_c2(compound_statement *self, c_transpiler2_ctx *ctx);
void *statement_to_c2(statement *self, c_transpiler2_ctx *ctx);
void *method_to_c2(method *self, c_transpiler2_ctx *ctx);
void *interface_to_c2(interface *self, c_transpiler2_ctx *ctx);
void *implementation_to_c2(implementation *self, c_transpiler2_ctx *ctx);
void *message_to_c2(message *self, c_transpiler2_ctx *ctx);
void *raw_to_c2(raw *self, c_transpiler2_ctx *ctx);
void *identifier_to_c2(identifier *self, c_transpiler2_ctx *ctx);
void *top_level_to_c2(top_level *self, c_transpiler2_ctx *ctx);
void *expr_to_c2(expr *self, c_transpiler2_ctx *ctx);
void *message_param_to_c2(message_param *self, c_transpiler2_ctx *ctx);
void *selector_to_c2(selector *self, c_transpiler2_ctx *ctx);
void *encode_to_c2(encode *self, c_transpiler2_ctx *ctx);
void *keyword_arg_to_c2(keyword_arg *self, c_transpiler2_ctx *ctx);
void *binop_expr_to_c2(binop_expr *self, c_transpiler2_ctx *ctx);
void *conditional_expr_to_c2(conditional_expr *self, c_transpiler2_ctx *ctx);
void *unary_op_expr_to_c2(unary_op_expr *self, c_transpiler2_ctx *ctx);
void *cast_expr_to_c2(cast_expr *self, c_transpiler2_ctx *ctx);


#endif // C_TRANSPILER_VISITOR_H
