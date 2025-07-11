#include "../ast.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "c_transpiler.h"

c_transpiler_ctx c_transpiler_ctx_init()
{
    c_transpiler_ctx output = (c_transpiler_ctx) {
        .tu = tmpfile(),
        .init = tmpfile()
    };
    output.current = output.tu;
    return output;
}

static void _dump_one(FILE *stream, char *label)
{
    printf("==== %s ====\n", label);
    fflush(stream);              // flush any buffered writes
    fseek(stream, 0, SEEK_SET);  // rewind to beginning

    char buf[1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), stream)) > 0) {
        fwrite(buf, 1, n, stdout);
    }
}

void c_transpiler_ctx_dump(c_transpiler_ctx *ctx) {
    _dump_one(ctx->tu, "impl");
    _dump_one(ctx->init, "init");
    printf("----------\n");

}


void *compound_statement_to_c(compound_statement *self, c_transpiler_ctx *ctx) {
    fprintf(ctx->current, "{\n");
    for (size_t i = 0; i < self->statements_count; ++i) {
        if (self->statements && self->statements[i] && self->statements[i]->base.accept)
            self->statements[i]->base.accept((ast*)self->statements[i], c_transpiler_visitor, ctx);
    }
    fprintf(ctx->current, "}\n");
    return NULL;
}

void *statement_to_c(statement *self, c_transpiler_ctx *ctx) {
    
    if (self->statement_type == is_expr && self->e_val) {
        self->e_val->base.accept((ast*)self->e_val, c_transpiler_visitor, ctx);
        fprintf(ctx->current, ";\n");
    } else if (self->statement_type == is_raw && self->r_val) {
        self->r_val->base.accept((ast*)self->r_val, c_transpiler_visitor, ctx);
    } else {
        fprintf(ctx->current, "(unknown_statement_type)");
    }
    return NULL;
}

void *method_to_c(method *self, c_transpiler_ctx *ctx) {

    fprintf(ctx->current, "%s __ObjcGenerated_", self->return_type);
    for (size_t i = 0; i < self->keyword_arg_count; ++i) {
        
        fprintf(ctx->current, "%s",self->keyword_args[i]->keyword);
    }
    fprintf(ctx->current, "(");

    if (self->method_type == member_method)
        fprintf(ctx->current, "struct %s * self, ", ctx->current_iface);
    fprintf(ctx->current, "SEL _cmd");
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
        self->body->accept(self->body, c_transpiler_visitor, ctx);
    return NULL;
}

void *interface_to_c(interface *self, c_transpiler_ctx *ctx) {
    ctx->current = ctx->tu;
    fprintf(ctx->current, "\n/****    IFACE START    *****/\n");
    fprintf(ctx->current, "\nstruct __ObjcGenerated_%s {\n /*todo: reserve bytes for parents?*/\n", self->name);
    ctx->current_iface = self->name;
    if (self->ivars)
    {
        size_t i = 0;
        while (i < self->ivars->statements_count)
        {
            self->ivars->statements[i]->base.accept((ast*)self->ivars->statements[i], c_transpiler_visitor, (void*)ctx);
            i += 1;
        }
    }
    fprintf(ctx->current, "\n};\n");
    
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], c_transpiler_visitor, ctx);
    }
    fprintf(ctx->current, "\n/****    IFACE END    *****/\n");
    return NULL;
}

void *implementation_to_c(implementation *self, c_transpiler_ctx *ctx) {
    ctx->current = ctx->tu;
    fprintf(ctx->current, "\n/****    IMPL START    *****/\n");
    for (int i = 0; i < self->method_count; ++i) {
        if (self->methods && self->methods[i] && self->methods[i]->base.accept)
            self->methods[i]->base.accept((ast*)self->methods[i], c_transpiler_visitor, ctx);
    }
    fprintf(ctx->current, "\n/****    IMPL END    *****/\n");
    return NULL;
}

void *message_to_c(message *self, c_transpiler_ctx *ctx) {
    fprintf(ctx->current, "objc_msgSend(");
    
    self->receiver->base.accept((ast*)self->receiver, c_transpiler_visitor, ctx);

    fprintf(ctx->current, ", ");


    for (int i = 0; i < self->params_count; ++i) {
        if (self->params && self->params[i] && self->params[i]->base.accept)
            self->params[i]->base.accept((ast*)self->params[i], c_transpiler_visitor, ctx);
    }
    fprintf(ctx->current, ")");
    return NULL;
}

void *raw_to_c(raw *self, c_transpiler_ctx *ctx) {
    size_t i = 0;
    while (isspace(self->source[i]))
        i += 1;
    fprintf(ctx->current, "%s", self->source + i);
    return NULL;
}

void *top_level_to_c(top_level *self, c_transpiler_ctx *ctx) {
    for (size_t i = 0; i < self->size; ++i) {
        self->childs[i]->accept(self->childs[i], c_transpiler_visitor, ctx);
    }
    ctx->current = ctx->tu;
    fprintf(ctx->current, "\nvoid __init() {\n");

    fprintf(ctx->current, "\n}\n");

    return NULL;
}

void *expr_to_c(expr *self, c_transpiler_ctx *ctx) {
    fprintf(ctx->current, " ");
    for (int i = 0; i < self->exprs_count; ++i) {
        if (self->exprs && self->exprs[i] && (self->exprs[i])->accept) {
            (self->exprs[i])->accept(self->exprs[i], c_transpiler_visitor, ctx);
        }
        fprintf(ctx->current, " ");
        if (i + 1 != self->exprs_count)
            fprintf(ctx->current, ", ");
    }
    return NULL;
}

void *message_param_to_c(message_param *self, c_transpiler_ctx *ctx) {
    fprintf(ctx->current, "%s", self->keyword);
    if (self->value && self->value->base.accept)
    {
        fprintf(ctx->current, ":");
        self->value->base.accept((ast*)self->value, c_transpiler_visitor, ctx);
    }
    return NULL;
}

void *selector_to_c(selector *self, c_transpiler_ctx *ctx) {
    fprintf(ctx->current, "sel_getUid(\"%s\")", self->str);
    return NULL;
}

void *encode_to_c(encode *self, c_transpiler_ctx *ctx) {
    fprintf(ctx->current, "@encode(%s)", self->type);
    return NULL;
}

void *keyword_arg_to_c(keyword_arg *self, c_transpiler_ctx *ctx) {
    fprintf(ctx->current, "%s", self->keyword);
    if (self->name && self->type) {
        fprintf(ctx->current, ":(%s)%s", self->type, self->name);
    }
    return NULL;
}

void *binop_expr_to_c(binop_expr *self, c_transpiler_ctx *ctx) {
    
    if (self->left && (self->left)->accept)
        (self->left)->accept(self->left, c_transpiler_visitor, ctx);
    fprintf(ctx->current, "%s", self->op);
    if (self->right && (self->right)->accept)
        (self->right)->accept(self->right, c_transpiler_visitor, ctx);
    return NULL;
}

void *conditional_expr_to_c(conditional_expr *self, c_transpiler_ctx *ctx) {
    if (self->test && (self->test)->accept)
        (self->test)->accept(self->test, c_transpiler_visitor, ctx);
    fprintf(ctx->current, "?");
    if (self->consequent && (self->consequent)->accept)
        (self->consequent)->accept(self->consequent, c_transpiler_visitor, ctx);
    fprintf(ctx->current, ":");
    if (self->alternate && (self->alternate)->accept)
        (self->alternate)->accept(self->alternate, c_transpiler_visitor, ctx);
    return NULL;
}

void *unary_op_expr_to_c(unary_op_expr *self, c_transpiler_ctx *ctx) {
    if (self->pos == unary_op_expr_prefix) {
        fprintf(ctx->current, "%s", self->op);
    } 
    (self->expr)->accept(self->expr, c_transpiler_visitor, ctx);
    if (self->pos == unary_op_expr_sufix) {
        fprintf(ctx->current, "%s", self->op);
        if (self->arg) {
            (self->arg)->accept(self->arg, c_transpiler_visitor, ctx);
            if (!strcmp(self->op, "("))
                fprintf(ctx->current, ")");
            if (!strcmp(self->op, "]"))
                fprintf(ctx->current, "]");
        }
    }
    return NULL;
}

void *cast_expr_to_c(cast_expr *self, c_transpiler_ctx *ctx) {

    fprintf(ctx->current, "(%s)", self->type);
    if (self->expr && (self->expr)->accept)
        (self->expr)->accept(self->expr, c_transpiler_visitor, ctx);
    return NULL;
}

ast_visitor c_transpiler_visitor = {
    .compound_statement = (void*) compound_statement_to_c,
    .statement          = (void*) statement_to_c,
    .method             = (void*) method_to_c,
    .interface          = (void*) interface_to_c,
    .implementation     = (void*) implementation_to_c,
    .message            = (void*) message_to_c,
    .raw                = (void*) raw_to_c,
    .top_level          = (void*) top_level_to_c,
    .expr               = (void*) expr_to_c,
    .message_param      = (void*) message_param_to_c,
    .selector           = (void*) selector_to_c,
    .encode             = (void*) encode_to_c,
    .keyword_arg        = (void*) keyword_arg_to_c,
    .binop_expr         = (void*) binop_expr_to_c,
    .conditional_expr   = (void*) conditional_expr_to_c,
    .unary_op_expr      = (void*) unary_op_expr_to_c,
    .cast_expr          = (void*) cast_expr_to_c,
};
