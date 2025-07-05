#include "ast.h"
#include <stdio.h>
#include <stdlib.h>


void *param_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.param_node((param_node*)self, arg);
}

void *method_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.method_node((method_node*)self, arg);
}

void *interface_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.interface_node((interface_node*)self, arg);
}

void *implementation_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.implementation_node((implementation_node*)self, arg);
}

void *message_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.message_node((message_node*)self, arg);
}

void *selector_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.selector_node((selector_node*)self, arg);
}

void *raw_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.raw_node((raw_node*)self, arg);
}

void *tu_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.tu_node((tu_node*)self, arg);
}

void *expr_node_accept(ast_node *self, ast_node_visitor visitor, void* arg) {
    return visitor.expr_node((expr_node*)self, arg);
}
