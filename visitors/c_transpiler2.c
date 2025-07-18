#include "../ast.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "c_transpiler2.h"
#include "identifier.h"

c_transpiler2_ctx c_transpiler2_ctx_init()
{
    c_transpiler2_ctx output = (c_transpiler2_ctx) {
        .iface = tmpfile(),
        .impl = tmpfile(),
        .init = tmpfile(),
        .classes_count = 0
    };
    output.current = output.impl;
    return output;
}

static void _dump_one(FILE *stream, char *label)
{
    printf("==== %s ====\n", label);
    fflush(stream);
    fseek(stream, 0, SEEK_SET);

    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), stream)) > 0) {
        fwrite(buf, 1, n, stdout);
    }
}

void c_transpiler2_ctx_dump(c_transpiler2_ctx *ctx) {
    _dump_one(ctx->iface, "iface");
    _dump_one(ctx->impl, "impl");
    _dump_one(ctx->init, "init");
    printf("----------\n");
}

void *compound_statement_to_c2(compound_statement *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, "{\n");
    for (size_t i = 0; i < self->statements_count; ++i) {
        if (self->statements && self->statements[i] && self->statements[i]->base.accept)
            self->statements[i]->base.accept((ast*)self->statements[i], c_transpiler2_visitor, ctx);
    }
    fprintf(ctx->current, "}\n");
    return NULL;
}

void *statement_to_c2(statement *self, c_transpiler2_ctx *ctx) {
    if (self->statement_type == is_expr && self->e_val) {
        self->e_val->base.accept((ast*)self->e_val, c_transpiler2_visitor, ctx);
        fprintf(ctx->current, ";\n");
    } else if (self->statement_type == is_raw && self->r_val) {
        self->r_val->base.accept((ast*)self->r_val, c_transpiler2_visitor, ctx);
    } else {
        fprintf(ctx->current, "(unknown_statement_type)");
    }
    return NULL;
}

// Helper: For a given method, resolve 'instancetype' to the class name for codegen (do not mutate the struct)
static const char *resolved_return_type(const method *m, const char *class_name) {
    if (m->return_type && strcmp(m->return_type, "instancetype") == 0) {
        // Use a static buffer for simplicity (not thread-safe, but fine for codegen)
        static char buf[256];
        snprintf(buf, sizeof(buf), "%s*", class_name);
        return buf;
    }
    return m->return_type;
}

void *method_to_c2(method *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, "%s __%s_", resolved_return_type(self, ctx->current_iface), ctx->current_iface);
    for (size_t i = 0; i < self->keyword_arg_count; ++i) {
        fprintf(ctx->current, "%s",self->keyword_args[i]->keyword);
    }
    fprintf(ctx->current, "(");
    fprintf(ctx->current, "%s * self", ctx->current_iface);
    if (self->keyword_arg_count >= 1 && self->keyword_args[0]->type && strlen(self->keyword_args[0]->type))
        fprintf(ctx->current, ", ");
    for (size_t i = 0; i < self->keyword_arg_count; ++i) {
        if (!self->keyword_args[i]->type || !strlen(self->keyword_args[i]->type))
            break ;
        fprintf(ctx->current, "%s %s",self->keyword_args[i]->type,self->keyword_args[i]->name);
        if (i+1 != self->keyword_arg_count)
            fprintf(ctx->current, ", ");
    }
    fprintf(ctx->current, ")");
    if (!self->body)
        fprintf(ctx->current, ";\n");
    else 
        self->body->accept(self->body, c_transpiler2_visitor, ctx);
    return NULL;
}

static void dump_function_ptr(method *m, c_transpiler2_ctx *ctx)
{
    fprintf(ctx->current, "%s(*", resolved_return_type(m, ctx->current_iface));
    for (size_t i = 0; i < m->keyword_arg_count; i += 1)
        fprintf(ctx->current, "%s", m->keyword_args[i]->keyword);
    fprintf(ctx->current, ")(");
    int is_static = 0;
    if (!is_static)
        fprintf(ctx->current, "%s*", ctx->current_iface);
    if (m->keyword_args[0]->type)
    {
        if (!is_static)
            fprintf(ctx->current, ", ");
        fprintf(ctx->current, "%s", m->keyword_args[0]->type);
    }
    for (size_t i = 1; i < m->keyword_arg_count; i += 1)
    {
        if (i == 1 && !is_static)
            fprintf(ctx->current, ", ");
        fprintf(ctx->current, "%s", m->keyword_args[i]->type);
        if (i+1 != m->keyword_arg_count)
          fprintf(ctx->current, ",");
    }
    if (m->keyword_arg_count == 1 && is_static)
        fprintf(ctx->current, "void");
    fprintf(ctx->current, ");\n");
}

static class_map *find(char *name, c_transpiler2_ctx *ctx)
{
    for (size_t i = 0; i < ctx->classes_count; i += 1)
    {
        if (!strcmp(name, ctx->classes[i]->name))
            return ctx->classes[i];
    }
    return NULL;
}

// Helper: Compare two methods by signature (name and argument types)
static int method_signature_equals(method *a, method *b) {
    if (!a || !b) return 0;
    if (a->keyword_arg_count != b->keyword_arg_count) return 0;
    for (size_t i = 0; i < a->keyword_arg_count; ++i) {
        if (strcmp(a->keyword_args[i]->keyword, b->keyword_args[i]->keyword) != 0)
            return 0;
        if ((a->keyword_args[i]->type && b->keyword_args[i]->type && strcmp(a->keyword_args[i]->type, b->keyword_args[i]->type) != 0) ||
            (a->keyword_args[i]->type == NULL && b->keyword_args[i]->type != NULL) ||
            (a->keyword_args[i]->type != NULL && b->keyword_args[i]->type == NULL))
            return 0;
    }
    return 1;
}

// Helper: Add method to list if not already present (by signature)
static void add_method_if_not_present(method **methods, size_t *count, method *m) {
    for (size_t i = 0; i < *count; ++i) {
        if (method_signature_equals(methods[i], m)) {
            methods[i] = m; // override parent with child
            return;
        }
    }
    methods[(*count)++] = m;
}

// Recursive helper: Collect all methods from parent chain and self
static void collect_all_methods(class_map *cls, method **out_methods, size_t *out_count) {
    if (cls->parent) {
        collect_all_methods(cls->parent, out_methods, out_count);
    }
    for (size_t i = 0; i < cls->methods_count; ++i) {
        add_method_if_not_present(out_methods, out_count, cls->methods[i]);
    }
}

// Helper: Recursively emit parent struct members (ivars and methods) before the child's, avoiding duplicate method pointers
static void emit_struct_members(class_map *cls, c_transpiler2_ctx *ctx, const char *child_name, interface *child_iface, method **emitted_methods, size_t *emitted_count) {
    if (cls->parent) {
        emit_struct_members(cls->parent, ctx, child_name, child_iface, emitted_methods, emitted_count);
    }
    // Emit parent ivars (if any)
    if (child_iface && child_iface->superclass_name && child_iface->ivars) {
        // Only emit ivars for the current class, not for parent (already emitted)
    } else if (child_iface && child_iface->ivars) {
        size_t i = 0;
        while (i < child_iface->ivars->statements_count) {
            fprintf(ctx->current, "    ");
            child_iface->ivars->statements[i]->base.accept((ast*)child_iface->ivars->statements[i], c_transpiler2_visitor, (void*)ctx);
            i += 1;
        }
    }
    // Do not emit method pointers here (vtable handles them)
}

// Helper: Find the class_map for a given interface name
static class_map *find_class_map(const char *name, c_transpiler2_ctx *ctx) {
    for (size_t i = 0; i < ctx->classes_count; ++i) {
        if (!strcmp(ctx->classes[i]->name, name))
            return ctx->classes[i];
    }
    return NULL;
}

void *interface_to_c2(interface *self, c_transpiler2_ctx *ctx) {
    ctx->current = ctx->iface;
    ctx->current_iface = self->name;
    ctx->classes[ctx->classes_count] = malloc(sizeof(class_map));
    ctx->classes[ctx->classes_count]->name = self->name;
    ctx->classes[ctx->classes_count]->methods_count = 0;
    ctx->classes[ctx->classes_count]->instancetypes_count = 0;
    if (self->superclass_name) {
        class_map *par = find(self->superclass_name, ctx);
        if (!par) {
            fprintf(stderr, "ERROR: Parent class not found.");
        }
        ctx->classes[ctx->classes_count]->parent = par;
    }
    for (size_t i = 0; i < self->method_count; i += 1) {
        ctx->classes[ctx->classes_count]->methods[ctx->classes[ctx->classes_count]->methods_count++] = self->methods[i];
        if (!strcmp(self->methods[i]->return_type, "instancetype")) {
            ctx->classes[ctx->classes_count]->instancetypes[ctx->classes[ctx->classes_count]->instancetypes_count++] = self->methods[i];
        }
    }
    // --- Use recursive struct member emission for binary compatibility ---
    fprintf(ctx->current, "\n/****    IFACE START    *****/\n");
    fprintf(ctx->current, "\ntypedef struct %s %s;\n", self->name, self->name);
    // Emit vtable struct
    fprintf(ctx->current, "struct %s_vtable {\n", self->name);
    // Emit vtable header fields
    const char *parent_vtable = self->superclass_name ? self->superclass_name : self->name;
    fprintf(ctx->current, "    size_t size;\n");
    fprintf(ctx->current, "    char *name;\n");
    fprintf(ctx->current, "    struct %s_vtable *super;\n", parent_vtable);
    // Collect all methods (including inherited)
    method *all_methods[255];
    size_t all_methods_count = 0;
    collect_all_methods(ctx->classes[ctx->classes_count], all_methods, &all_methods_count);
    for (size_t i = 0; i < all_methods_count; ++i) {
        const char *ret_type = all_methods[i]->return_type && strcmp(all_methods[i]->return_type, "instancetype") == 0
            ? resolved_return_type(all_methods[i], self->name)
            : all_methods[i]->return_type;
        fprintf(ctx->current, "    %s(*", ret_type);
        for (size_t k = 0; k < all_methods[i]->keyword_arg_count; k += 1)
            fprintf(ctx->current, "%s", all_methods[i]->keyword_args[k]->keyword);
        fprintf(ctx->current, ")(");
        int is_static = 0;
        if (!is_static)
            fprintf(ctx->current, "%s*", self->name);
        if (all_methods[i]->keyword_args[0]->type) {
            if (!is_static)
                fprintf(ctx->current, ", ");
            fprintf(ctx->current, "%s", all_methods[i]->keyword_args[0]->type);
        }
        for (size_t k = 1; k < all_methods[i]->keyword_arg_count; k += 1) {
            if (k == 1 && !is_static)
                fprintf(ctx->current, ", ");
            fprintf(ctx->current, "%s", all_methods[i]->keyword_args[k]->type);
            if (k+1 != all_methods[i]->keyword_arg_count)
                fprintf(ctx->current, ",");
        }
        if (all_methods[i]->keyword_arg_count == 1 && is_static)
            fprintf(ctx->current, "void");
        fprintf(ctx->current, ");\n");
    }
    fprintf(ctx->current, "};\n");
    // Emit class struct (with vtable pointer and ivars only)
    fprintf(ctx->current, "struct %s {\n", self->name);
    fprintf(ctx->current, "    struct %s_vtable* vtable;\n", self->name);
    // Emit this class's ivars
    if (self->ivars) {
        size_t i = 0;
        while (i < self->ivars->statements_count) {
            fprintf(ctx->current, "    ");
            self->ivars->statements[i]->base.accept((ast*)self->ivars->statements[i], c_transpiler2_visitor, (void*)ctx);
            i += 1;
        }
    }
    fprintf(ctx->current, "\n};\n");
    fprintf(ctx->current, "extern %s * %sClass;\n", self->name, self->name);
    // Generate method declarations for all methods (child overrides parent)
    for (size_t i = 0; i < all_methods_count; ++i) {
        all_methods[i]->base.accept((ast*)all_methods[i], c_transpiler2_visitor, ctx);
    }
    fprintf(ctx->current, "\n/****    IFACE END    *****/\n");
    ctx->classes_count += 1;
    return NULL;
}

// Helper: Find the class name that implements a given method (by signature), searching up the parent chain
static const char *find_implementing_class(class_map *cls, method *target) {
    if (!cls) return NULL;
    for (size_t i = 0; i < cls->methods_count; ++i) {
        if (method_signature_equals(cls->methods[i], target)) {
            return cls->name;
        }
    }
    if (cls->parent) return find_implementing_class(cls->parent, target);
    return NULL;
}

void *implementation_to_c2(implementation *self, c_transpiler2_ctx *ctx) {
    ctx->current = ctx->impl;
    fprintf(ctx->current, "\n/****    IMPL START    *****/\n");
    // Emit method implementations
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], c_transpiler2_visitor, ctx);
    }
    // Emit vtable instance
    // Find class_map for this implementation
    class_map *cls = find((char*)self->name, ctx);
    if (cls) {
        // Collect all methods (including inherited)
        method *all_methods[255];
        size_t all_methods_count = 0;
        collect_all_methods(cls, all_methods, &all_methods_count);
        // Find parent vtable name
        fprintf(ctx->current, "\nstruct %s_vtable %sClassVtable = {\n", self->name, self->name);
        fprintf(ctx->current, "    .size = sizeof(%s),\n", self->name);
        fprintf(ctx->current, "    .name = \"%s\",\n", self->name);
        if (cls->parent)
            fprintf(ctx->current, "    .super = &%sClassVtable,\n", cls->parent->name);
        else
            fprintf(ctx->current, "    .super = NULL,\n");
        // Emit method pointers
        for (size_t i = 0; i < all_methods_count; ++i) {
            // Find the class that implements this method
            const char *impl_class = find_implementing_class(cls, all_methods[i]);
            fprintf(ctx->current, "    .");
            for (size_t k = 0; k < all_methods[i]->keyword_arg_count; ++k)
                fprintf(ctx->current, "%s", all_methods[i]->keyword_args[k]->keyword);
            if (impl_class) {
                fprintf(ctx->current, " = (void*)&__%s_", impl_class);
                for (size_t k = 0; k < all_methods[i]->keyword_arg_count; ++k)
                    fprintf(ctx->current, "%s", all_methods[i]->keyword_args[k]->keyword);
            } else {
                fprintf(ctx->current, " = (void*)0");
            }
            fprintf(ctx->current, ",\n");
        }
        fprintf(ctx->current, "};\n");
        // Emit singleton instance and pointer
        fprintf(ctx->current, "%s %sClassImpl = { .vtable = &%sClassVtable };\n", self->name, self->name, self->name);
        fprintf(ctx->current, "%s *%sClass = &%sClassImpl;\n", self->name, self->name, self->name);
    }
    fprintf(ctx->current, "\n/****    IMPL END    *****/\n");
    return NULL;
}


void *message_to_c2(message *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, " ");
    // Check if receiver is 'super'
    int is_super = 0;
    if (self->receiver && self->receiver->exprs_count == 1 && self->receiver->exprs[0]) {
        ast *maybe_id = self->receiver->exprs[0];
        if (maybe_id->accept) {
            identifier *id = (identifier*)maybe_id;
            if (id->base.accept == identifier_accept && id->source && strcmp(id->source, "super") == 0) {
                is_super = 1;
            }
        }
    }
    if (is_super) {
        // Emit self->vtable->super->method(self, ...)
        fprintf(ctx->current, "self->vtable->super->");
        for (int i = 0; i < self->params_count; ++i) {
            fprintf(ctx->current, "%s", self->params[i]->keyword);
        }
        fprintf(ctx->current, "(self");
        if (self->params_count && self->params[0]->value) {
            fprintf(ctx->current, ", ");
            for (int i = 0; i < self->params_count; ++i) {
                self->params[i]->value->base.accept((ast*)self->params[i]->value, c_transpiler2_visitor, ctx);
                if (i+1 != self->params_count)
                    fprintf(ctx->current, ", ");
            }
        }
        fprintf(ctx->current, ")");
        return NULL;
    }

    self->receiver->base.accept((ast*)self->receiver, c_transpiler2_visitor, ctx);

    char *id = self->receiver->base.accept((ast*)self->receiver, identifier_visitor, ctx);
    int is_static = 0;

    if (id && !strcmp(id, "expr"))
    {
        if (((expr*)self->receiver)->exprs_count == 1)
        {
            id = self->receiver->exprs[0]->accept((ast*)self->receiver->exprs[0], identifier_visitor, ctx);
           
            if (id && !strcmp(id, "identifier"))
            {
                identifier *candidate_class = (identifier*) self->receiver->exprs[0];
                size_t i = 0;
                while (i < ctx->classes_count)
                {
                    if (!strcmp(ctx->classes[i]->name, candidate_class->source))
                    {
                        fprintf(ctx->current, "Class");
                        is_static = 1;
                        break ;
                    }
                    i += 1;
                }
            }
        }
    }

    fprintf(ctx->current, "->vtable->");


    for (int i = 0; i < self->params_count; ++i) {
        fprintf(ctx->current, "%s", self->params[i]->keyword);
    }
    fprintf(ctx->current, "(");

    self->receiver->base.accept((ast*)self->receiver, c_transpiler2_visitor, ctx);
    if (is_static)
        fprintf(ctx->current, "Class");


    if (self->params_count && self->params[0]->value)
    {
        fprintf(ctx->current, ", ");
        for (int i = 0; i < self->params_count; ++i) {
            self->params[i]->value->base.accept((ast*)self->params[i]->value, c_transpiler2_visitor, ctx);
            if (i+1 != self->params_count)
                fprintf(ctx->current, ", ");
        }
    }
    fprintf(ctx->current, ")");
    return NULL;
}

void *raw_to_c2(raw *self, c_transpiler2_ctx *ctx) {
    size_t i = 0;
    while (isspace(self->source[i]))
        i += 1;
    fprintf(ctx->current, "%s", self->source + i);
    return NULL;
}

void *identifier_to_c2(identifier *self, c_transpiler2_ctx *ctx) {
    size_t i = 0;
    while (isspace(self->source[i]))
        i += 1;
    fprintf(ctx->current, " %s", self->source + i);
    return NULL;
}

void *top_level_to_c2(top_level *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->init, "\nvoid __init() {\n");
    for (size_t i = 0; i < self->size; ++i) {
        ctx->current = ctx->impl;
        self->childs[i]->accept(self->childs[i], c_transpiler2_visitor, ctx);
    }
    fprintf(ctx->init, "}\n");
    return NULL;
}

void *expr_to_c2(expr *self, c_transpiler2_ctx *ctx) {
    for (int i = 0; i < self->exprs_count; ++i) {
        if (self->exprs && self->exprs[i] && (self->exprs[i])->accept) {
            (self->exprs[i])->accept(self->exprs[i], c_transpiler2_visitor, ctx);
        }
        if (i + 1 != self->exprs_count)
            fprintf(ctx->current, ", ");
    }
    return NULL;
}

void *message_param_to_c2(message_param *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, "%s", self->keyword);
    if (self->value && self->value->base.accept)
    {
        fprintf(ctx->current, ":");
        self->value->base.accept((ast*)self->value, c_transpiler2_visitor, ctx);
    }
    return NULL;
}

void *selector_to_c2(selector *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, "sel_getUid(\"%s\")", self->str);
    return NULL;
}

void *encode_to_c2(encode *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, "@encode(%s)", self->type);
    return NULL;
}

void *keyword_arg_to_c2(keyword_arg *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, "%s", self->keyword);
    if (self->name && self->type) {
        fprintf(ctx->current, ":(%s)%s", self->type, self->name);
    }
    return NULL;
}

void *binop_expr_to_c2(binop_expr *self, c_transpiler2_ctx *ctx) {
    if (self->left && (self->left)->accept)
        (self->left)->accept(self->left, c_transpiler2_visitor, ctx);
    fprintf(ctx->current, "%s", self->op);
    if (self->right && (self->right)->accept)
        (self->right)->accept(self->right, c_transpiler2_visitor, ctx);
    return NULL;
}

void *conditional_expr_to_c2(conditional_expr *self, c_transpiler2_ctx *ctx) {
    if (self->test && (self->test)->accept)
        (self->test)->accept(self->test, c_transpiler2_visitor, ctx);
    fprintf(ctx->current, "?");
    if (self->consequent && (self->consequent)->accept)
        (self->consequent)->accept(self->consequent, c_transpiler2_visitor, ctx);
    fprintf(ctx->current, ":");
    if (self->alternate && (self->alternate)->accept)
        (self->alternate)->accept(self->alternate, c_transpiler2_visitor, ctx);
    return NULL;
}

void *unary_op_expr_to_c2(unary_op_expr *self, c_transpiler2_ctx *ctx) {
    if (self->pos == unary_op_expr_prefix) {
        fprintf(ctx->current, "%s", self->op);
    } 
    (self->expr)->accept(self->expr, c_transpiler2_visitor, ctx);
    if (self->pos == unary_op_expr_sufix) {
        fprintf(ctx->current, "%s", self->op);
        if (self->arg) {
            (self->arg)->accept(self->arg, c_transpiler2_visitor, ctx);
            if (!strcmp(self->op, "("))
                fprintf(ctx->current, ")");
            if (!strcmp(self->op, "]"))
                fprintf(ctx->current, "]");
        }
    }
    return NULL;
}

void *cast_expr_to_c2(cast_expr *self, c_transpiler2_ctx *ctx) {
    fprintf(ctx->current, "(%s)", self->type);
    if (self->expr && (self->expr)->accept)
        (self->expr)->accept(self->expr, c_transpiler2_visitor, ctx);
    return NULL;
}

ast_visitor c_transpiler2_visitor = {
    .compound_statement = (void*) compound_statement_to_c2,
    .statement          = (void*) statement_to_c2,
    .method             = (void*) method_to_c2,
    .interface          = (void*) interface_to_c2,
    .implementation     = (void*) implementation_to_c2,
    .message            = (void*) message_to_c2,
    .raw                = (void*) raw_to_c2,
    .identifier         = (void*) identifier_to_c2,
    .top_level          = (void*) top_level_to_c2,
    .expr               = (void*) expr_to_c2,
    .message_param      = (void*) message_param_to_c2,
    .selector           = (void*) selector_to_c2,
    .encode             = (void*) encode_to_c2,
    .keyword_arg        = (void*) keyword_arg_to_c2,
    .binop_expr         = (void*) binop_expr_to_c2,
    .conditional_expr   = (void*) conditional_expr_to_c2,
    .unary_op_expr      = (void*) unary_op_expr_to_c2,
    .cast_expr          = (void*) cast_expr_to_c2,
};
