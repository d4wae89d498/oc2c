#include <stdio.h>
#include "transpiler.h"


ast_node_visitor transpiler = {
    .param_node = &param_node_transpile,
    .method_node = &method_node_transpile,
    .interface_node = &interface_node_transpile,
    .implementation_node = &implementation_node_transpile,
    .message_node = &message_node_transpile,
    .selector_node = &selector_node_transpile,
    .raw_node = &raw_node_transpile,
    .tu_node = &tu_node_transpile,
    .expr_node = &expr_node_transpile
};

void *param_node_transpile(param_node *self, void *ctx) {
    return NULL;
}

void *method_node_transpile(method_node *self, void *ctx) {
    return NULL;
}

void *interface_node_transpile(interface_node *self, void *ctx) {
    return NULL;
}

void *implementation_node_transpile(implementation_node *self, void *ctx) {
    return NULL;
}

void *message_node_transpile(message_node *self, void *ctx) {
    return NULL;
}

void *selector_node_transpile(selector_node *self, void *ctx) {
    return NULL;
}

void *raw_node_transpile(raw_node *self, void *ctx) {
    return NULL;
}

void *tu_node_transpile(tu_node *self, void *ctx) {
    return NULL;
}

void *expr_node_transpile(expr_node *self, void *ctx) {
    return NULL;
}