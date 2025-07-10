#ifndef TRANSPILER_VISITOR_H
# define TRANSPILER_VISITOR_H
#include "./../ast.h"

extern ast_visitor transpiler;

void *compound_statement_transpile(compound_statement *self, void *ctx);
void *method_transpile(method *self, void *ctx);
void *interface_transpile(interface *self, void *ctx);
void *implementation_transpile(implementation *self, void *ctx);
void *message_transpile(message *self, void *ctx);
void *raw_transpile(raw *self, void *ctx);
void *top_level_transpile(top_level *self, void *ctx);
void *expr_transpile(expr *self, void *ctx);
void *statement_transpile(statement *self, void *ctx);
void *message_param_transpile(message_param *self, void *ctx);
void *selector_transpile(selector *self, void *ctx);
void *encode_transpile(encode *self, void *ctx);

#endif