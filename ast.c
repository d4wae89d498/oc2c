#include "ast.h"
#include <stdio.h>
#include <stdlib.h>


void *message_param_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.message_param((message_param*)self, arg);
}

void *method_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.method((method*)self, arg);
}

void *interface_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.interface((interface*)self, arg);
}

void *implementation_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.implementation((implementation*)self, arg);
}

void *message_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.message((message*)self, arg);
}

void *raw_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.raw((raw*)self, arg);
}

void *top_level_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.tu((top_level*)self, arg);
}

void *expr_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.expr((expr*)self, arg);
}

void *statement_accept(ast *self, ast_visitor visitor, void *arg) {
    return visitor.statement((statement*) self, arg);
}
