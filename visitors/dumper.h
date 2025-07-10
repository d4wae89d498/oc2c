#ifndef DUMPER_VISITOR_H
#define DUMPER_VISITOR_H

#include "../ast.h"

extern ast_visitor dumper_visitor;

void *compound_statement_dump(compound_statement *self, void *ctx);
void *statement_dump(statement *self, void *ctx);
void *method_dump(method *self, void *ctx);
void *interface_dump(interface *self, void *ctx);
void *implementation_dump(implementation *self, void *ctx);
void *message_dump(message *self, void *ctx);
void *raw_dump(raw *self, void *ctx);
void *top_level_dump(top_level *self, void *ctx);
void *expr_dump(expr *self, void *ctx);
void *message_param_dump(message_param *self, void *ctx);
void *selector_dump(selector *self, void *ctx);
void *encode_dump(encode *self, void *ctx);
void *keyword_arg_dump(keyword_arg *self, void *ctx);
void *binop_expr_dump(binop_expr *self, void *ctx);
void *conditional_expr_dump(conditional_expr *self, void *ctx);
void *unary_op_expr_dump(unary_op_expr *self, void *ctx);
void *cast_expr_dump(cast_expr *self, void *ctx);

#endif // DUMPER_VISITOR_H
