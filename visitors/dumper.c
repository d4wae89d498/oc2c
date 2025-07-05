#ifndef VISITORS_DUMPER_C
#define VISITORS_DUMPER_C

#include "../ast.h"
#include <stdio.h>
#include "dumper.h"

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) putchar(' ');
}

void *param_node_dump(param_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("param: type=%s name=%s\n", self->type ? self->type : "(null)", self->name ? self->name : "(null)");
    return NULL;
}

void *method_node_dump(method_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("method: selector=%s return_type=%s\n", self->selector ? self->selector : "(null)", self->return_type ? self->return_type : "(null)");
    for (int i = 0; i < self->param_count; ++i) {
        if (self->params && self->params[i] && self->params[i] && self->params[i]->base.accept)
            self->params[i]->base.accept((ast_node*)self->params[i], dumper_visitor, ctx);
    }
    return NULL;
}

void *interface_node_dump(interface_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("interface: name=%s superclass=%s\n", self->name ? self->name : "(null)", self->superclass_name ? self->superclass_name : "(null)");
    printf("ivars: \n");
    for (int i = 0; i < self->ivar_count; ++i) {
        printf("   - ivar[%i]: %s\n", i, self->ivars[i]);
    }
    printf("methods:\n");
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast_node*)self->methods[i], dumper_visitor, ctx);
    }
    printf("---\n");
    return NULL;
}

void *implementation_node_dump(implementation_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("implementation: name=%s superclass=%s\n", self->name ? self->name : "(null)", self->superclass_name ? self->superclass_name : "(null)");
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast_node*)self->methods[i], dumper_visitor, ctx);
    }
    return NULL;
}

void *message_node_dump(message_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("message: selector=%s\n", self->selector ? self->selector : "(null)");
    // TODO: dump args
    return NULL;
}

void *selector_node_dump(selector_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("selector: name=%s\n", self->name ? self->name : "(null)");
    return NULL;
}

void *raw_node_dump(raw_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("raw: '%s'\n", self->source ? self->source : "(null)");
    return NULL;
}

void *tu_node_dump(tu_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("translation_unit (%li)\n", self->size);
    for (size_t i = 0; i < self->size; ++i) {
        if (self->childs && self->childs[i] && self->childs[i]->accept) {
            printf("- %li\n", i);
            self->childs[i]->accept(self->childs[i], dumper_visitor, ctx);
        }
    }
    return NULL;
}

void *expr_node_dump(expr_node *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("expr\n");
    for (int i = 0; i < self->child_count; ++i) {
        if (self->children && self->children[i] && self->children[i]->accept)
            self->children[i]->accept(self->children[i], dumper_visitor, ctx);
    }
    return NULL;
}

ast_node_visitor dumper_visitor = {
    .param_node = param_node_dump,
    .method_node = method_node_dump,
    .interface_node = interface_node_dump,
    .implementation_node = implementation_node_dump,
    .message_node = message_node_dump,
    .selector_node = selector_node_dump,
    .raw_node = raw_node_dump,
    .tu_node = tu_node_dump,
    .expr_node = expr_node_dump
};

#endif // VISITORS_DUMPER_C 