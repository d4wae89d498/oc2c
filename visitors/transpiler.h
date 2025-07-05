#ifndef TRANSPILER_VISITOR_H
# define TRANSPILER_VISITOR_H
#include "./../ast.h"

extern ast_node_visitor transpiler;

void *param_node_transpile(param_node *self, void *ctx);
void *method_node_transpile(method_node *self, void *ctx);
void *interface_node_transpile(interface_node *self, void *ctx);
void *implementation_node_transpile(implementation_node *self, void *ctx);
void *message_node_transpile(message_node *self, void *ctx);
void *selector_node_transpile(selector_node *self, void *ctx);
void *raw_node_transpile(raw_node *self, void *ctx);
void *tu_node_transpile(tu_node *self, void *ctx);
void *expr_node_transpile(expr_node *self, void *ctx);

#endif