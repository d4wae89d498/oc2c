#include <stdio.h>
#include "transpiler.h"


ast_visitor transpiler = {
    .param = &param_transpile,
    .method = &method_transpile,
    .interface = &interface_transpile,
    .implementation = &implementation_transpile,
    .message = &message_transpile,
    .selector = &selector_transpile,
    .raw = &raw_transpile,
    .tu = &tu_transpile,
    .expr = &expr_transpile
};

void *param_transpile(param *self, void *ctx) {
    return NULL;
}

void *method_transpile(method *self, void *ctx) {
    return NULL;
}

void *interface_transpile(interface *self, void *ctx) {
    return NULL;
}

void *implementation_transpile(implementation *self, void *ctx) {
    return NULL;
}

void *message_transpile(message *self, void *ctx) {
    return NULL;
}

void *selector_transpile(selector *self, void *ctx) {
    return NULL;
}

void *raw_transpile(raw *self, void *ctx) {
    return NULL;
}

void *tu_transpile(tu *self, void *ctx) {
    return NULL;
}

void *expr_transpile(expr *self, void *ctx) {
    return NULL;
}