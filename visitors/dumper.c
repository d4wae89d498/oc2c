#ifndef VISITORS_DUMPER_C
#define VISITORS_DUMPER_C

#include "../ast.h"
#include <stdio.h>
#include "dumper.h"

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) putchar(' ');
}

void *param_dump(param *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("param: type=%s name=%s\n", self->type ? self->type : "(null)", self->name ? self->name : "(null)");
    return NULL;
}

void *method_dump(method *self, void *ctx) {
    
    return NULL;
}

void *interface_dump(interface *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("interface: name=%s superclass=%s\n", self->name ? self->name : "(null)", self->superclass_name ? self->superclass_name : "(null)");
    printf("ivars: \n");
    for (int i = 0; i < self->ivar_count; ++i) {
        printf("   - ivar[%i]: %s\n", i, self->ivars[i]);
    }
    printf("methods:\n");
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], dumper_visitor, ctx);
    }
    printf("---\n");
    return NULL;
}

void *implementation_dump(implementation *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("implementation: name=%s superclass=%s\n", self->name ? self->name : "(null)", self->superclass_name ? self->superclass_name : "(null)");
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], dumper_visitor, ctx);
    }
    return NULL;
}

void *message_dump(message *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("message: selector=%s\n", self->selector ? self->selector : "(null)");
    // TODO: dump args
    return NULL;
}


void *raw_dump(raw *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("raw: '%s'\n", self->source ? self->source : "(null)");
    return NULL;
}

void *tu_dump(tu *self, void *ctx) {
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

void *expr_dump(expr *self, void *ctx) {
    unsigned long long indent = (unsigned long long)ctx;
    print_indent(indent); printf("expr\n");
    for (int i = 0; i < self->child_count; ++i) {
        if (self->children && self->children[i] && self->children[i]->accept)
            self->children[i]->accept(self->children[i], dumper_visitor, ctx);
    }
    return NULL;
}

ast_visitor dumper_visitor = {
    .param = param_dump,
    .method = method_dump,
    .interface = interface_dump,
    .implementation = implementation_dump,
    .message = message_dump,
    .raw = raw_dump,
    .tu = tu_dump,
    .expr = expr_dump
};

#endif // VISITORS_DUMPER_C 