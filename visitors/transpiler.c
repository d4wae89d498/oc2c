#include "../ast.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "transpiler.h"

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) putchar(' ');
}

void *compound_statement_transpile(compound_statement *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;

    print_indent(indent); printf("{\n");
    for (size_t i = 0; i < self->statements_count; ++i) {
        if (self->statements && self->statements[i] && self->statements[i]->base.accept)
            self->statements[i]->base.accept((ast*)self->statements[i], transpiler_visitor, (void*)(indent + 2));
    }
    print_indent(indent); printf("}\n");
    return NULL;
}

void *statement_transpile(statement *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    
    print_indent(indent);
    if (self->statement_type == is_expr && self->e_val) {
        self->e_val->base.accept((ast*)self->e_val, transpiler_visitor, (void*)(indent + 2));
        printf(";\n");
    } else if (self->statement_type == is_raw && self->r_val) {
        self->r_val->base.accept((ast*)self->r_val, transpiler_visitor, (void*)(indent + 2));
    } else {
        printf("(unknown_statement_type)");
    }
    return NULL;
}

void *method_transpile(method *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("%s (%s) ", self->method_type == static_method ? "+" : "-", self->return_type);
    for (size_t i = 0; i < self->keyword_arg_count; ++i) {
        if (self->keyword_args && self->keyword_args[i] && self->keyword_args[i]->base.accept)
            self->keyword_args[i]->base.accept((ast*)self->keyword_args[i], transpiler_visitor, (void*)(indent + 2));
    }
    if (self->body && (self->body)->accept)
        (self->body)->accept(self->body, transpiler_visitor, (void*)(indent + 2));
    else
        printf(";");
    return NULL;
}

void *interface_transpile(interface *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("\n@interface %s %s %s\n", self->name, self->superclass_name ? ":" : "", self->superclass_name ? self->superclass_name : "");
    if (self->ivars) {
        printf(" \n");
        self->ivars->base.accept((ast*)self->ivars, transpiler_visitor, (void*)(indent + 2));        
        printf("\n");
    }
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], transpiler_visitor, (void*)(indent + 2));
    }
    printf("\n@end\n");
    return NULL;
}

void *implementation_transpile(implementation *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    printf("\n@implementation %s \n", self->name);
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], transpiler_visitor, (void*)(indent + 2));
    }
    printf("\n@end\n");
    return NULL;
}

void *message_transpile(message *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("[");
    if (self->receiver && (self->receiver->base.accept))
        self->receiver->base.accept((ast*)self->receiver, transpiler_visitor, (void*)(indent + 2));
    for (int i = 0; i < self->params_count; ++i) {
        if (self->params && self->params[i] && self->params[i]->base.accept)
            self->params[i]->base.accept((ast*)self->params[i], transpiler_visitor, (void*)(indent + 2));
    }
    printf("]");
    return NULL;
}

void *raw_transpile(raw *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); 
    size_t i = 0;
    while (isspace(self->source[i]))
        i += 1;
    printf("%s", self->source + i);
    return NULL;
}

void *identifier_transpile(identifier *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); 
    size_t i = 0;
    while (isspace(self->source[i]))
        i += 1;
    printf("%s", self->source + i);
    return NULL;
}

void *top_level_transpile(top_level *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    for (size_t i = 0; i < self->size; ++i) {
        self->childs[i]->accept(self->childs[i], transpiler_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *expr_transpile(expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    for (int i = 0; i < self->exprs_count; ++i) {
        if (self->exprs && self->exprs[i] && (self->exprs[i])->accept) {
            (self->exprs[i])->accept(self->exprs[i], transpiler_visitor, (void*)(indent + 2));
        }
        printf(" ");
        if (i + 1 != self->exprs_count)
            printf(", ");
    }
    return NULL;
}

void *message_param_transpile(message_param *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    printf("%s", self->keyword);
    if (self->value && self->value->base.accept)
    {
        printf(":");
        self->value->base.accept((ast*)self->value, transpiler_visitor, (void*)(indent + 2));
    }
    return NULL;
}

void *selector_transpile(selector *self, void *ctx) {
    printf("@selector(%s)", self->str);
    return NULL;
}

void *encode_transpile(encode *self, void *ctx) {
    printf("@encode(%s)", self->type);
    return NULL;
}

void *keyword_arg_transpile(keyword_arg *self, void *ctx) {
    printf("%s", self->keyword);
    if (self->name && self->type) {
        printf(":(%s)%s", self->type, self->name);
    }
    return NULL;
}

void *binop_expr_transpile(binop_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    
    if (self->left && (self->left)->accept)
        (self->left)->accept(self->left, transpiler_visitor, (void*)(indent + 4));
    printf("%s", self->op);
    if (self->right && (self->right)->accept)
        (self->right)->accept(self->right, transpiler_visitor, (void*)(indent + 4));
    return NULL;
}

void *conditional_expr_transpile(conditional_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); 
    if (self->test && (self->test)->accept)
        (self->test)->accept(self->test, transpiler_visitor, (void*)(indent + 2));
    printf("?");
    if (self->consequent && (self->consequent)->accept)
        (self->consequent)->accept(self->consequent, transpiler_visitor, (void*)(indent + 2));
    printf(":");
    if (self->alternate && (self->alternate)->accept)
        (self->alternate)->accept(self->alternate, transpiler_visitor, (void*)(indent + 2));
    return NULL;
}

void *unary_op_expr_transpile(unary_op_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent);
    if (self->pos == unary_op_expr_prefix) {
        printf("%s", self->op);
    } 
    (self->expr)->accept(self->expr, transpiler_visitor, (void*)(indent + 2));
    if (self->pos == unary_op_expr_sufix) {
        printf("%s", self->op);
        if (self->arg) {
            (self->arg)->accept(self->arg, transpiler_visitor, (void*)(indent + 2));
            if (!strcmp(self->op, "("))
                printf(")");
            if (!strcmp(self->op, "]"))
                printf("]");
        }
    }
    return NULL;
}

void *cast_expr_transpile(cast_expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;

    printf("(%s)", self->type);
    if (self->expr && (self->expr)->accept)
        (self->expr)->accept(self->expr, transpiler_visitor, (void*)(indent + 2));
    return NULL;
}

ast_visitor transpiler_visitor = {
    .compound_statement = compound_statement_transpile,
    .statement          = statement_transpile,
    .method             = method_transpile,
    .interface          = interface_transpile,
    .implementation     = implementation_transpile,
    .message            = message_transpile,
    .raw                = raw_transpile,
    .identifier         = identifier_transpile,
    .top_level          = top_level_transpile,
    .expr               = expr_transpile,
    .message_param      = message_param_transpile,
    .selector           = selector_transpile,
    .encode             = encode_transpile,
    .keyword_arg        = keyword_arg_transpile,
    .binop_expr         = binop_expr_transpile,
    .conditional_expr   = conditional_expr_transpile,
    .unary_op_expr      = unary_op_expr_transpile,
    .cast_expr          = cast_expr_transpile,
};
