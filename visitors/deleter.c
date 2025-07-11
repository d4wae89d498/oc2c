#include "../ast.h"
#include <stdio.h>
#include <string.h>
#include "deleter.h"

void *compound_statement_delete(compound_statement *self, void *ctx) {
    for (size_t i = 0; i < self->statements_count; ++i) {
        if (self->statements && self->statements[i] && self->statements[i]->base.accept)
            self->statements[i]->base.accept((ast*)self->statements[i], deleter_visitor, ctx);
    }
    free(self);
    return NULL;
}

void *statement_delete(statement *self, void *ctx) {
    if (self->statement_type == is_expr && self->e_val) {
        self->e_val->base.accept((ast*)self->e_val, deleter_visitor, ctx);
    } else if (self->statement_type == is_raw && self->r_val) {
        self->r_val->base.accept((ast*)self->r_val, deleter_visitor, ctx);
    } else if (self->statement_type == is_cp && self->cp_val) {
        self->cp_val->base.accept((ast*)self->cp_val, deleter_visitor, ctx);
    }
    free(self);
    return NULL;
}

void *method_delete(method *self, void *ctx) {
    for (size_t i = 0; i < self->keyword_arg_count; ++i) {
        if (self->keyword_args && self->keyword_args[i] && self->keyword_args[i]->base.accept)
            self->keyword_args[i]->base.accept((ast*)self->keyword_args[i], deleter_visitor, ctx);
    }
    if (self->body && (self->body)->accept)
        (self->body)->accept(self->body, deleter_visitor, ctx);
    free(self->return_type);
    free(self);
    return NULL;
}

void *interface_delete(interface *self, void *ctx) {
    if (self->ivars && self->ivars->base.accept)
        self->ivars->base.accept((ast*)self->ivars, deleter_visitor, ctx);
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], deleter_visitor, ctx);
    }
    free(self->name);
    free(self->superclass_name);
    free(self);
    return NULL;
}

void *implementation_delete(implementation *self, void *ctx) {
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], deleter_visitor, ctx);
    }
    free(self->name);
    free(self->superclass_name);
    free(self);
    return NULL;
}

void *message_delete(message *self, void *ctx) {
    if (self->receiver && (self->receiver->base.accept))
        self->receiver->base.accept((ast*)self->receiver, deleter_visitor, ctx);
    for (int i = 0; i < self->params_count; ++i) {
        if (self->params && self->params[i] && self->params[i]->base.accept)
            self->params[i]->base.accept((ast*)self->params[i], deleter_visitor, ctx);
    }
    free(self);
    return NULL;
}

void *raw_delete(raw *self, void *ctx) {
    free(self->source);
    free(self);
    return NULL;
}

void *top_level_delete(top_level *self, void *ctx) {
    for (size_t i = 0; i < self->size; ++i) {
        if (self->childs[i] && self->childs[i]->accept)
            self->childs[i]->accept(self->childs[i], deleter_visitor, ctx);
    }
    free(self);
    return NULL;
}

void *expr_delete(expr *self, void *ctx) {
    for (int i = 0; i < self->exprs_count; ++i) {
        if (self->exprs && self->exprs[i] && (self->exprs[i])->accept)
            (self->exprs[i])->accept(self->exprs[i], deleter_visitor, ctx);
    }
    free(self);
    return NULL;
}

void *message_param_delete(message_param *self, void *ctx) {
    if (self->value && self->value->base.accept)
        self->value->base.accept((ast*)self->value, deleter_visitor, ctx);
    free(self->keyword);
    free(self);
    return NULL;
}

void *selector_delete(selector *self, void *ctx) {
    free(self->str);
    free(self);
    return NULL;
}

void *encode_delete(encode *self, void *ctx) {
    free(self->type);
    free(self);
    return NULL;
}

void *keyword_arg_delete(keyword_arg *self, void *ctx) {
    free(self->keyword);
    free(self->type);
    free(self->name);
    free(self);
    return NULL;
}

void *binop_expr_delete(binop_expr *self, void *ctx) {
    if (self->left && (self->left)->accept)
        (self->left)->accept(self->left, deleter_visitor, ctx);
    if (self->right && (self->right)->accept)
        (self->right)->accept(self->right, deleter_visitor, ctx);
    free(self->op);
    free(self);
    return NULL;
}

void *conditional_expr_delete(conditional_expr *self, void *ctx) {
    if (self->test && (self->test)->accept)
        (self->test)->accept(self->test, deleter_visitor, ctx);
    if (self->consequent && (self->consequent)->accept)
        (self->consequent)->accept(self->consequent, deleter_visitor, ctx);
    if (self->alternate && (self->alternate)->accept)
        (self->alternate)->accept(self->alternate, deleter_visitor, ctx);
    free(self);
    return NULL;
}

void *unary_op_expr_delete(unary_op_expr *self, void *ctx) {
    if (self->expr && (self->expr)->accept)
        (self->expr)->accept(self->expr, deleter_visitor, ctx);
    if (self->arg && (self->arg)->accept)
        (self->arg)->accept(self->arg, deleter_visitor, ctx);
    free(self->op);
    free(self);
    return NULL;
}

void *cast_expr_delete(cast_expr *self, void *ctx) {
    if (self->expr && (self->expr)->accept)
        (self->expr)->accept(self->expr, deleter_visitor, ctx);
    free(self->type);
    free(self);
    return NULL;
}

ast_visitor deleter_visitor = {
    .compound_statement = compound_statement_delete,
    .statement          = statement_delete,
    .method             = method_delete,
    .interface          = interface_delete,
    .implementation     = implementation_delete,
    .message            = message_delete,
    .raw                = raw_delete,
    .top_level          = top_level_delete,
    .expr               = expr_delete,
    .message_param      = message_param_delete,
    .selector           = selector_delete,
    .encode             = encode_delete,
    .keyword_arg        = keyword_arg_delete,
    .binop_expr         = binop_expr_delete,
    .conditional_expr   = conditional_expr_delete,
    .unary_op_expr      = unary_op_expr_delete,
    .cast_expr          = cast_expr_delete,
};
