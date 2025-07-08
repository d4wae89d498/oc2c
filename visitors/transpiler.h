#ifndef TRANSPILER_VISITOR_H
# define TRANSPILER_VISITOR_H
#include "./../ast.h"

extern ast_visitor transpiler;

void *param_transpile(param *self, void *ctx);
void *method_transpile(method *self, void *ctx);
void *interface_transpile(interface *self, void *ctx);
void *implementation_transpile(implementation *self, void *ctx);
void *message_transpile(message *self, void *ctx);
void *raw_transpile(raw *self, void *ctx);
void *tu_transpile(tu *self, void *ctx);
void *expr_transpile(expr *self, void *ctx);

#endif