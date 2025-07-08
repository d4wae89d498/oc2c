#include "ast.h"
#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// --- Parser context and state for string parsing ---

struct parser_ctx_state {
    size_t pos;
    size_t depth;
};

static parser_ctx_state save_ctx_state(parser_ctx *ctx) {
    return (parser_ctx_state) {
        .pos = ctx->pos,
        .depth = ctx->depth + 1
    };
}

static void restore_ctx_state(parser_ctx *ctx, parser_ctx_state state) {
    ctx->pos = state.pos;
    ctx->depth = state.depth - 1;
}

// --- String parsing helpers ---

// Skip whitespace
static void skip_ws(parser_ctx *ctx) {
    while (ctx->input[ctx->pos] == ' ' || ctx->input[ctx->pos] == '\t' || ctx->input[ctx->pos] == '\n' || ctx->input[ctx->pos] == '\r') {
        ctx->pos++;
    }
}

// Advances if 'what' is in input, returning a malloc'd copy if matched, or NULL otherwise
char *parse_str(parser_ctx *ctx, char *what) {
    skip_ws(ctx);
    if (what) {
        size_t len = strlen(what);
        if (strncmp(ctx->input + ctx->pos, what, len) == 0) {
            ctx->pos += len;
            return what;
        }
        return NULL;
    }
    return NULL;
}

// Parse a C identifier (returns malloc'd string or NULL)
char *parse_identifier(parser_ctx *ctx) {
    skip_ws(ctx);
    size_t start = ctx->pos;
    if ((ctx->input[ctx->pos] >= 'a' && ctx->input[ctx->pos] <= 'z') ||
        (ctx->input[ctx->pos] >= 'A' && ctx->input[ctx->pos] <= 'Z') ||
        ctx->input[ctx->pos] == '_') {
        ctx->pos++;
        while ((ctx->input[ctx->pos] >= 'a' && ctx->input[ctx->pos] <= 'z') ||
               (ctx->input[ctx->pos] >= 'A' && ctx->input[ctx->pos] <= 'Z') ||
               (ctx->input[ctx->pos] >= '0' && ctx->input[ctx->pos] <= '9') ||
               ctx->input[ctx->pos] == '_') {
            ctx->pos++;
        }
        size_t len = ctx->pos - start;
        char *out = malloc(len + 1);
        memcpy(out, ctx->input + start, len);
        out[len] = '\0';
        return out;
    }
    return NULL;
}

// --- try_parse macro and helper ---
#define try_parse(ptr, ctx, parser, ...) \
    _try_parse(#parser, ctx, save_ctx_state(ctx), parser(ctx __VA_ARGS__), (void**)ptr)

static void *_try_parse(char *name, parser_ctx *ctx, parser_ctx_state state, void *result, void **ptr) {
    if (!result) {
        restore_ctx_state(ctx, state);
        printf("[%li] Parsing failed for %s [%.15s]\n", ctx->depth, name, ctx->input + ctx->pos);
    } else {
        printf("[%li] Parsing ok for %s [%.15s]\n",ctx->depth, name, ctx->input + ctx->pos);
    }
    if(ptr)
        *ptr = result;
    return result;
}

char *parse_parens_content(parser_ctx *ctx) {
    size_t start = ctx->pos;
    int parens = 1; // we assume '(' was already consumed

    while (ctx->pos < ctx->length && parens > 0) {
        char c = ctx->input[ctx->pos++];

        if (c == '(') parens++;
        else if (c == ')') parens--;
    }

    if (parens != 0)
        return NULL; // unbalanced parentheses

    size_t end = ctx->pos - 1; // exclude closing ')'

    size_t len = end - start;
    char *content = malloc(len + 1);
    memcpy(content, &ctx->input[start], len);
    content[len] = '\0';

    return content;
}


// --- EOF check ---
static int ctx_eof(parser_ctx *ctx) {
    skip_ws(ctx);
    return ctx->input[ctx->pos] == '\0';
}

char peek_char(parser_ctx *ctx)
{
    return *(ctx->input + ctx->pos);
}

void advance(parser_ctx *ctx)
{
    ctx->pos += 1;
}

char *parse_ivar(parser_ctx *ctx) {
    skip_ws(ctx);

    size_t start = ctx->pos;
    while (!ctx_eof(ctx)) {
        char c = peek_char(ctx);
        if (c == ';') {
            advance(ctx);
            break;
        }
        advance(ctx);
    }

    size_t end = ctx->pos;
    if (start == end) return NULL;

    size_t len = end - start;
    char *ivar_str = malloc(len + 1);
    memcpy(ivar_str, ctx->input + start, len);
    ivar_str[len] = '\0';

    skip_ws(ctx);

    return ivar_str;
}


param *parse_param(parser_ctx *ctx) {
    char *type = NULL, *name = NULL;

    if (!try_parse(&type, ctx, parse_identifier,) || !try_parse(&name, ctx, parse_identifier,))
        return NULL;

    make_ast(param, node, {
        .type = type,
        .name = name
    });
    return node;
}

method *parse_method(parser_ctx *ctx) {
    int is_class_method = 0;
    if (try_parse(0, ctx, parse_str, ,"+" )) {
        is_class_method = 1;
    } else if (try_parse(0, ctx, parse_str,, "-" )) {
        is_class_method = 0;
    } else {
        return NULL;
    }

    skip_ws(ctx);

    if (!try_parse(0, ctx, parse_str,, "(")) return NULL;
    char *return_type = try_parse(0, ctx, parse_parens_content, );
    if (!return_type) return NULL;

    skip_ws(ctx);

    char *selector = try_parse(0, ctx, parse_identifier);
    if (!selector) return NULL;

    struct param **params = NULL;
    int param_count = 0;

    skip_ws(ctx);

    while (try_parse(0, ctx, parse_str,, ":")) {
        skip_ws(ctx);

        if (!try_parse(0, ctx, parse_str,, "(")) return NULL;
        char *param_type = parse_parens_content(ctx);
        if (!param_type) return NULL;

        skip_ws(ctx);

        char *param_name = try_parse(0, ctx, parse_identifier);
        if (!param_name) return NULL;

        make_ast(param, param, {
            .type = param_type,
            .name = param_name
        });

        params = realloc(params, sizeof(*params) * (param_count + 1));
        params[param_count++] = param;

        skip_ws(ctx);
    }

    if (!try_parse(0, ctx, parse_str,, ";")) return NULL;

    make_ast(method, node, {
        .is_class_method = is_class_method,
        .return_type = return_type,
        .selector = selector,
        .params = params,
        .param_count = param_count,
        .body = NULL
    });
    return node;
}


implementation *parse_implementation(parser_ctx *ctx) {
    char *name = try_parse(0, ctx, parse_identifier,);
    if (!name)
        return NULL;
    make_ast(implementation, node, {
        .name = name,
        .superclass_name = NULL,
        .methods = NULL,
        .method_count = 0
    });

    return node;
}

message *parse_message(parser_ctx *ctx) {
    make_ast(message, node, {
        .selector = NULL,
        .args = NULL,
        .arg_count = 0
    });
    return node;
}

expr *parse_expr(parser_ctx *ctx) {
    make_ast(expr, node, {
        .children = NULL,
        .child_count = 0
    });
    return node;
}

tu *parse_tu(parser_ctx *ctx) {
    make_ast(tu, node, {
        .childs = malloc(sizeof(ast *)),
        .size = 0
    });

    while (!ctx_eof(ctx)) {

        ast *child = NULL;

        /* === Inline parse_raw logic === */
        size_t raw_start = ctx->pos;
        size_t buf_cap = 256, buf_len = 0;
        char *buf = malloc(buf_cap);
        int in_str = 0, in_comm = 0;
        char str_del = 0;

        while (ctx->input[ctx->pos]) {
            char c  = ctx->input[ctx->pos];
            char c2 = ctx->input[ctx->pos+1];

            /* === Lookahead for [id id] ObjC message === */
            if (!in_str && !in_comm && c == '[') {
                parser_ctx_state st = save_ctx_state(ctx);
                ctx->pos++; skip_ws(ctx);
                char *id1 = parse_identifier(ctx);
                skip_ws(ctx);
                char *id2 = id1 ? parse_identifier(ctx) : NULL;
                int is_msg = (id1 && id2);
                free(id1); free(id2);
                restore_ctx_state(ctx, st);
                printf("f1\n");
                if (is_msg) break;
            }

            /* === Lookahead for @interface or @implementation === */
            if (!in_str && !in_comm && c == '@') {
                parser_ctx_state st = save_ctx_state(ctx);
                ctx->pos++; skip_ws(ctx);
                if (    parse_str(ctx, "interface")
                     || parse_str(ctx, "implementation")) {
                    restore_ctx_state(ctx, st);  
                    
                    printf("f2\n");

                    break;
                }
                restore_ctx_state(ctx, st);
            }

            /* === String handling === */
            if (!in_comm && (c=='"' || c=='\'')) {
                if (!in_str) { in_str=1; str_del=c; }
                else if (c==str_del) {
                    size_t back=ctx->pos; int esc=0;
                    while (back>raw_start && ctx->input[--back]=='\\') esc++;
                    if ((esc%2)==0) in_str=0;
                }
                buf[buf_len++] = c; ctx->pos++;
            }
            else if (in_str) {
                buf[buf_len++] = c; ctx->pos++;
            }
            /* === Comment handling === */
            else if (!in_str && c=='/' && c2=='/') {
                in_comm=1; buf[buf_len++]=c; buf[buf_len++]=c2; ctx->pos+=2;
            }
            else if (in_comm==1) {
                buf[buf_len++] = c; ctx->pos++;
                if (c=='\n') in_comm=0;
            }
            else if (!in_str && c=='/' && c2=='*') {
                in_comm=2; buf[buf_len++]=c; buf[buf_len++]=c2; ctx->pos+=2;
            }
            else if (in_comm==2) {
                buf[buf_len++] = c;
                if (c=='*' && ctx->input[ctx->pos+1]=='/') {
                    buf[buf_len++]='/'; ctx->pos+=2; in_comm=0;
                } else ctx->pos++;
            }
            /* === Preprocessor lines === */
            else if (!in_str && !in_comm && c=='#') {
                size_t line_start=ctx->pos;
                while (ctx->input[ctx->pos] && ctx->input[ctx->pos]!='\n') ctx->pos++;
                if (ctx->input[ctx->pos]=='\n') ctx->pos++;
                size_t len=ctx->pos-line_start;
                if (buf_len+len+1>buf_cap) { buf_cap=(buf_len+len+1)*2; buf=realloc(buf,buf_cap); }
                memcpy(buf+buf_len, ctx->input+line_start, len); buf_len+=len;
            }
            /* === Default case: copy one char === */
            else {
                buf[buf_len++] = c; ctx->pos++;
            }

            if (buf_len+1 > buf_cap) { buf_cap*=2; buf=realloc(buf,buf_cap); }
        }

        printf("%.15s\n", ctx->input + ctx->pos);
        /* If we collected raw text before ObjC marker, emit raw */
        if (ctx->pos > raw_start) {
            buf[buf_len] = '\0';
            make_ast(raw, node, { .source=buf });
            child = (ast*)node;
            
        }
        /* Otherwise try ObjC make_asts directly */
        else if (   try_parse(&child, ctx, parse_interface,)
                 || try_parse(&child, ctx, parse_implementation,))
        {
            
            free(buf); /* discard unused raw buffer */
        }
        else {
            /* nothing left to parse */
            free(buf);
            break;
        }


        if (node->size)
            node->childs = realloc(node->childs, sizeof(ast *) * (node->size+1));

        node->childs[node->size++] = child;
    }

    printf("%li\n", node->size);
    return node;
}


interface *parse_interface(parser_ctx *ctx) {
    if (!try_parse(0, ctx, parse_str,, "@interface"))
        return NULL;

    skip_ws(ctx);

    char *identifier = try_parse(0, ctx, parse_identifier,);
    if (!identifier)
        return NULL;

    skip_ws(ctx);

    // Optional superclass: ": SuperClass"
    char *superclass_name = NULL;
    if (try_parse(0, ctx, parse_str,, ":")) {
        skip_ws(ctx);
        superclass_name = try_parse(0, ctx, parse_identifier,);
        if (!superclass_name)
            return NULL;
        skip_ws(ctx);
    }

    // Optional ivar block: { ivar declarations }
    char **ivars = NULL;
    int ivar_count = 0;
    if (try_parse(0, ctx, parse_str,, "{")) {
        skip_ws(ctx);
        while (!try_parse(0, ctx, parse_str ,, "}")) {
            char *ivar = parse_ivar(ctx);
            if (!ivar) return NULL;
            ivars = realloc(ivars, sizeof(*ivars) * (ivar_count + 1));
            ivars[ivar_count++] = ivar;
            skip_ws(ctx);
        }
        skip_ws(ctx);
    }

    printf("testin methods-----\n");
    // Methods: accumulate until @end
    struct method **methods = NULL;
    int method_count = 0;
    while (!try_parse(0, ctx, parse_str,, "@end")) {
        struct method *method = try_parse(0, ctx, parse_method, );
        if (!method) return NULL;
        methods = realloc(methods, sizeof(*methods) * (method_count + 1));
        methods[method_count++] = method;
        skip_ws(ctx);
    }

    make_ast(interface, node, {
        .name = identifier,
        .superclass_name = superclass_name,
        .ivars = ivars,
        .ivar_count = ivar_count,
        .methods = methods,
        .method_count = method_count
    });
    return node;
}