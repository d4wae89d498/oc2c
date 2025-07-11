#ifndef DELETER_VISITOR_H
# define DELETER_VISITOR_H

# include "../ast.h"

extern ast_visitor deleter_visitor;

void *compound_statement_delete(compound_statement *self, void *ctx);
void *statement_delete(statement *self, void *ctx);
void *method_delete(method *self, void *ctx);
void *interface_delete(interface *self, void *ctx);
void *implementation_delete(implementation *self, void *ctx);
void *message_delete(message *self, void *ctx);
void *raw_delete(raw *self, void *ctx);
void *top_level_delete(top_level *self, void *ctx);
void *expr_delete(expr *self, void *ctx);
void *message_param_delete(message_param *self, void *ctx);
void *selector_delete(selector *self, void *ctx);
void *encode_delete(encode *self, void *ctx);
void *keyword_arg_delete(keyword_arg *self, void *ctx);
void *binop_expr_delete(binop_expr *self, void *ctx);
void *conditional_expr_delete(conditional_expr *self, void *ctx);
void *unary_op_expr_delete(unary_op_expr *self, void *ctx);
void *cast_expr_delete(cast_expr *self, void *ctx);

#endif // deleter_VISITOR_H
