#ifndef C_TRANSPILE_VISITOR_H
#define C_TRANSPILE_VISITOR_H

#include "../ast.h"
#include <stdio.h>

extern ast_visitor c_transpiler_visitor;

typedef struct c_transpiler_ctx 
{
    FILE    *tu;
    FILE    *init;

    FILE    *current;
    char    *current_iface;
} c_transpiler_ctx;

c_transpiler_ctx    c_transpiler_ctx_init();
void                c_transpiler_ctx_dump(c_transpiler_ctx *ctx);

void *compound_statement_to_c(compound_statement *self, c_transpiler_ctx *ctx);
void *statement_to_c(statement *self, c_transpiler_ctx *ctx);
void *method_to_c(method *self, c_transpiler_ctx *ctx);
void *interface_to_c(interface *self, c_transpiler_ctx *ctx);
void *implementation_to_c(implementation *self, c_transpiler_ctx *ctx);
void *message_to_c(message *self, c_transpiler_ctx *ctx);
void *raw_to_c(raw *self, c_transpiler_ctx *ctx);
void *top_level_to_c(top_level *self, c_transpiler_ctx *ctx);
void *expr_to_c(expr *self, c_transpiler_ctx *ctx);
void *message_param_to_c(message_param *self, c_transpiler_ctx *ctx);
void *selector_to_c(selector *self, c_transpiler_ctx *ctx);
void *encode_to_c(encode *self, c_transpiler_ctx *ctx);
void *keyword_arg_to_c(keyword_arg *self, c_transpiler_ctx *ctx);
void *binop_expr_to_c(binop_expr *self, c_transpiler_ctx *ctx);
void *conditional_expr_to_c(conditional_expr *self, c_transpiler_ctx *ctx);
void *unary_op_expr_to_c(unary_op_expr *self, c_transpiler_ctx *ctx);
void *cast_expr_to_c(cast_expr *self, c_transpiler_ctx *ctx);


#endif // TRANSPILER_VISITOR_H
