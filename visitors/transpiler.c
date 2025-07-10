#include <stdio.h>
#include "transpiler.h"

ast_visitor transpiler = {
    .compound_statement = compound_statement_transpile,
    .method = method_transpile,
    .interface = interface_transpile,
    .implementation = implementation_transpile,
    .message = message_transpile,
    .raw = raw_transpile,
    .top_level = top_level_transpile,
    .expr = expr_transpile,
    .statement = statement_transpile,
    .message_param = message_param_transpile,
    .selector = selector_transpile,
    .encode = encode_transpile
};

void *compound_statement_transpile(compound_statement *self, void *ctx) { return NULL; }
void *method_transpile(method *self, void *ctx) { return NULL; }
void *interface_transpile(interface *self, void *ctx) { return NULL; }
void *implementation_transpile(implementation *self, void *ctx) { return NULL; }
void *message_transpile(message *self, void *ctx) { return NULL; }
void *raw_transpile(raw *self, void *ctx) { return NULL; }
void *top_level_transpile(top_level *self, void *ctx) { return NULL; }
void *expr_transpile(expr *self, void *ctx) { return NULL; }
void *statement_transpile(statement *self, void *ctx) { return NULL; }
void *message_param_transpile(message_param *self, void *ctx) { return NULL; }
void *selector_transpile(selector *self, void *ctx) { return NULL; }
void *encode_transpile(encode *self, void *ctx) { return NULL; }