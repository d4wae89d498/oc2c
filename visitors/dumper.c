#ifndef VISITORS_DUMPER_C
#define VISITORS_DUMPER_C

#include "../ast.h"
#include <stdio.h>
#include <string.h>
#include "dumper.h"

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) putchar(' ');
}

void *compound_statement_dump(compound_statement *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("compound_statement (%zu statements)\n", self->statements_count);
    for (size_t i = 0; i < self->statements_count; ++i) {
        if (self->statements && self->statements[i] && self->statements[i]->base.accept)
            self->statements[i]->base.accept((ast*)self->statements[i], dumper_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *statement_dump(statement *self, void *ctx) {
    printf("____________\n");
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    if (self->statement_type == is_expr && self->e_val) {
        printf("statement: expr\n");
        self->e_val->base.accept((ast*)self->e_val, dumper_visitor, (void*)(indent + 2));
    } else if (self->statement_type == is_raw && self->r_val) {
        printf("statement: raw\n");
        self->r_val->base.accept((ast*)self->r_val, dumper_visitor, (void*)(indent + 2));
    } else {
        printf("statement: (unknown)\n");
    }
    return NULL;
}

void *method_dump(method *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("method: type=%s return_type=%s keyword_arg_count=%zu\n", 
           self->method_type == static_method ? "+" : "-",
           self->return_type ? self->return_type : "(null)",
           self->keyword_arg_count);
    for (size_t i = 0; i < self->keyword_arg_count; ++i) {
        if (self->keyword_args && self->keyword_args[i] && self->keyword_args[i]->base.accept)
            self->keyword_args[i]->base.accept((ast*)self->keyword_args[i], dumper_visitor, (void*)(indent + 2));
    }
    if (self->body && (self->body)->accept)
        (self->body)->accept(self->body, dumper_visitor, (void*)(indent + 2));
    return NULL;
}

void *interface_dump(interface *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("interface: name=%s superclass=%s\n",
           self->name ? self->name : "(null)",
           self->superclass_name ? self->superclass_name : "(null)");
    print_indent(indent); printf("ivars:\n");
    if (self->ivars && self->ivars->base.accept)
        self->ivars->base.accept((ast*)self->ivars, dumper_visitor, (void*)(indent + 2));
    print_indent(indent); printf("methods (%d):\n", self->method_count);
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], dumper_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *implementation_dump(implementation *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("implementation: name=%s superclass=%s\n",
           self->name ? self->name : "(null)",
           self->superclass_name ? self->superclass_name : "(null)");
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], dumper_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *message_dump(message *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("message\n");
    if (self->receiver && (self->receiver->base.accept))
        self->receiver->base.accept((ast*)self->receiver, dumper_visitor, (void*)(indent + 2));
    for (int i = 0; i < self->params_count; ++i) {
        if (self->params && self->params[i] && self->params[i]->base.accept)
            self->params[i]->base.accept((ast*)self->params[i], dumper_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *raw_dump(raw *self, void *ctx) {
    printf("dumping raw...\n");
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("raw: '%s'\n", self->source ? self->source : "(null)");
    return NULL;
}

void *identifier_dump(identifier *self, void *ctx) {
    printf("dumping raw...\n");
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("raw: '%s'\n", self->source ? self->source : "(null)");
    return NULL;
}

void *top_level_dump(top_level *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("top_level (%zu)\n", self->size);
    for (size_t i = 0; i < self->size; ++i) {
        self->childs[i]->accept(self->childs[i], dumper_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *expr_dump(expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("expr (%d)\n", self->exprs_count);
    for (int i = 0; i < self->exprs_count; ++i) {
        if (self->exprs && self->exprs[i] && (self->exprs[i])->accept)
            (self->exprs[i])->accept(self->exprs[i], dumper_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *message_param_dump(message_param *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("message_param: keyword=%s\n", self->keyword ? self->keyword : "(null)");
    if (self->value && self->value->base.accept)
        self->value->base.accept((ast*)self->value, dumper_visitor, (void*)(indent + 2));
    return NULL;
}

void *selector_dump(selector *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("selector: %s\n", self->str ? self->str : "(null)");
    return NULL;
}

void *encode_dump(encode *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("encode: type=%s\n", self->type ? self->type : "(null)");
    return NULL;
}

void *keyword_arg_dump(keyword_arg *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("keyword_arg: keyword=%s type=%s name=%s\n",
           self->keyword ? self->keyword : "(null)",
           self->type    ? self->type    : "(null)",
           self->name    ? self->name    : "(null)");
    return NULL;
}

void *binop_expr_dump(binop_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("binop_expr: op=%s\n", self->op ? self->op : "(null)");
    print_indent(indent + 2); printf("left=");
    if (self->left && (self->left)->accept)
        (self->left)->accept(self->left, dumper_visitor, (void*)(indent + 4));
    print_indent(indent + 2); printf("right=");
    if (self->right && (self->right)->accept)
        (self->right)->accept(self->right, dumper_visitor, (void*)(indent + 4));
    return NULL;
}

void *conditional_expr_dump(conditional_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("conditional_expr\n");
    if (self->test && (self->test)->accept)
        (self->test)->accept(self->test, dumper_visitor, (void*)(indent + 2));
    if (self->consequent && (self->consequent)->accept)
        (self->consequent)->accept(self->consequent, dumper_visitor, (void*)(indent + 2));
    if (self->alternate && (self->alternate)->accept)
        (self->alternate)->accept(self->alternate, dumper_visitor, (void*)(indent + 2));
    return NULL;
}

void *unary_op_expr_dump(unary_op_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("unary_op_expr: op=%s pos=%s\n",
           self->op ? self->op : "(null)",
           self->pos == unary_op_expr_prefix ? "prefix" : "suffix");    
    printf("expr=\n");
    if (self->expr && (self->expr)->accept)
    {
        (self->expr)->accept(self->expr, dumper_visitor, (void*)(indent + 2));
    }
    printf("arg= %p\n", self->arg);
    if (self->arg && (self->arg)->accept)
    {
        (self->arg)->accept(self->arg, dumper_visitor, (void*)(indent + 2));
    }
    printf("unary_op_expr\n");
    return NULL;
}

void *cast_expr_dump(cast_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("cast_expr: type=%s\n", self->type ? self->type : "(null)");
    if (self->expr && (self->expr)->accept)
        (self->expr)->accept(self->expr, dumper_visitor, (void*)(indent + 2));
    return NULL;
}

ast_visitor dumper_visitor = {
    .compound_statement = compound_statement_dump,
    .statement          = statement_dump,
    .method             = method_dump,
    .interface          = interface_dump,
    .implementation     = implementation_dump,
    .message            = message_dump,
    .raw                = raw_dump,
    .identifier         = identifier_dump,
    .top_level          = top_level_dump,
    .expr               = expr_dump,
    .message_param      = message_param_dump,
    .selector           = selector_dump,
    .encode             = encode_dump,
    .keyword_arg        = keyword_arg_dump,
    .binop_expr         = binop_expr_dump,
    .conditional_expr   = conditional_expr_dump,
    .unary_op_expr      = unary_op_expr_dump,
    .cast_expr          = cast_expr_dump,
};

#endif // VISITORS_DUMPER_C
