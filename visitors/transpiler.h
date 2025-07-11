#ifndef TRANSPILER_VISITOR_H
#define TRANSPILER_VISITOR_H

#include "../ast.h"

extern ast_visitor transpiler_visitor;

void *compound_statement_transpile(compound_statement *self, void *ctx);
void *statement_transpile(statement *self, void *ctx);
void *method_transpile(method *self, void *ctx);
void *interface_transpile(interface *self, void *ctx);
void *implementation_transpile(implementation *self, void *ctx);
void *message_transpile(message *self, void *ctx);
void *raw_transpile(raw *self, void *ctx);
void *top_level_transpile(top_level *self, void *ctx);
void *expr_transpile(expr *self, void *ctx);
void *message_param_transpile(message_param *self, void *ctx);
void *selector_transpile(selector *self, void *ctx);
void *encode_transpile(encode *self, void *ctx);
void *keyword_arg_transpile(keyword_arg *self, void *ctx);
void *binop_expr_transpile(binop_expr *self, void *ctx);
void *conditional_expr_transpile(conditional_expr *self, void *ctx);
void *unary_op_expr_transpile(unary_op_expr *self, void *ctx);
void *cast_expr_transpile(cast_expr *self, void *ctx);

#endif // TRANSPILER_VISITOR_H
