#include "../ast.h"
#include <stdio.h>
#include <string.h>
#include "identifier.h"

void *compound_statement_identify(compound_statement *self, void *ctx) {
    return "compound_statement";
}

void *statement_identify(statement *self, void *ctx) {
    return "statement";
}

void *method_identify(method *self, void *ctx) {
    return "method";
}

void *interface_identify(interface *self, void *ctx) {
    return "interface";
}

void *implementation_identify(implementation *self, void *ctx) {
    return "implementation";
}

void *message_identify(message *self, void *ctx) {
    return "message";
}

void *raw_identify(raw *self, void *ctx) {
    return "raw";
}

void *identifier_identify(identifier *self, void *ctx) {
    return "identifier";
}

void *top_level_identify(top_level *self, void *ctx) {
    return "top_level";
}

void *expr_identify(expr *self, void *ctx) {
    return "expr";
}

void *message_param_identify(message_param *self, void *ctx) {
    return "message_param";
}

void *selector_identify(selector *self, void *ctx) {
    return "selector";
}

void *encode_identify(encode *self, void *ctx) {
    return "encode";
}

void *keyword_arg_identify(keyword_arg *self, void *ctx) {
    return "keyword_arg";
}

void *binop_expr_identify(binop_expr *self, void *ctx) {
    return "binop_expr";
}

void *conditional_expr_identify(conditional_expr *self, void *ctx) {
    return "conditional_expr";
}

void *unary_op_expr_identify(unary_op_expr *self, void *ctx) {
    return "unary_op_expr";
}

void *cast_expr_identify(cast_expr *self, void *ctx) {
    return "cast_expr";
}

ast_visitor identifier_visitor = {
    .compound_statement = compound_statement_identify,
    .statement          = statement_identify,
    .method             = method_identify,
    .interface          = interface_identify,
    .implementation     = implementation_identify,
    .message            = message_identify,
    .raw                = raw_identify,
    .identifier         = identifier_identify,
    .top_level          = top_level_identify,
    .expr               = expr_identify,
    .message_param      = message_param_identify,
    .selector           = selector_identify,
    .encode             = encode_identify,
    .keyword_arg        = keyword_arg_identify,
    .binop_expr         = binop_expr_identify,
    .conditional_expr   = conditional_expr_identify,
    .unary_op_expr      = unary_op_expr_identify,
    .cast_expr          = cast_expr_identify,
};
