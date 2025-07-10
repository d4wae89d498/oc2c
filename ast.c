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
    return visitor.top_level((top_level*)self, arg);
}

void *expr_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.expr((expr*)self, arg);
}

void *statement_accept(ast *self, ast_visitor visitor, void *arg) {
    return visitor.statement((statement*) self, arg);
}

void *selector_accept(ast *self, ast_visitor visitor, void *arg) {
    return visitor.selector((selector*) self, arg);
}

void *encode_accept(ast *self, ast_visitor visitor, void *arg) {
    return visitor.encode((encode*) self, arg);
}

void *compound_statement_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.compound_statement((compound_statement*)self, arg);
}

void *binop_expr_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.binop_expr((binop_expr*)self, arg);
}

void *conditional_expr_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.conditional_expr((conditional_expr*)self, arg);
}

void *unary_op_expr_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.unary_op_expr((unary_op_expr*)self, arg);
}

void *cast_expr_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.cast_expr((cast_expr*)self, arg);
}

void *keyword_arg_accept(ast *self, ast_visitor visitor, void* arg) {
    return visitor.keyword_arg((keyword_arg*)self, arg);
}