#include "ast.h"
#include "parser.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include "./_strdup.h"

//////////////////////////////////////////////////////////
//////////////  CONTEXT                        ///////////
//////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////
//////////////  HELPERS                        ///////////
//////////////////////////////////////////////////////////

static void skip_whitespace(parser_ctx *ctx) {
    while (isspace(ctx->input[ctx->pos])) 
        ctx->pos++;
}

static void parse_skip_whitespace(parser_ctx *ctx) {
    return skip_whitespace(ctx);
}

//////

static char *exact_str(parser_ctx *ctx, char *what) {
    skip_whitespace(ctx);
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

static char *parse_exact_str(parser_ctx *ctx, char *what) {
    return exact_str(ctx, what);
}

//////////////////////////////////////////////////////////
//////////////  BACKTRACKING                   ///////////
//////////////////////////////////////////////////////////

// --- try_parse macro and helper ---
#define try_parse(ptr, ctx, parser, ...) \
    _try_parse(#parser, ctx, save_ctx_state(ctx), parse_ ## parser(ctx __VA_ARGS__), (void**)ptr)

static void *_try_parse(char *name, parser_ctx *ctx, parser_ctx_state state, void *result, void **ptr) {
    skip_whitespace(ctx);
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


#define try_parse_all(ctx, parser, args, min, max, ...)       \
    parser_ctx_state state =  save_ctx_state(ctx);            \
    void ** args = malloc(sizeof(void*));                     \
    int     args ## _count = 0;                               \
    int     args ## _success = 0;                             \
    {                                                         \
        ast  *current;                                        \
        while ((current = try_parse(0, ctx, parser __VA_ARGS__)))\
        {                                                     \
            *args = current;                                  \
            args ## _count += 1;                              \
            args = realloc(args, args ## _count * sizeof(void*)); \
        }                                                     \
    }                                                         \
    if ( args ## _count >= min &&  args ## _count <= max)     \
    args ## _success = 1;                                     \
    else                                                      \
        restore_ctx_state(ctx, state);



//////////////////////////////////////////////////////////
//////////////  LEXERS                         ///////////
//////////////////////////////////////////////////////////

char *parse_identifier(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    if (ctx->pos < ctx->length && (isalpha(ctx->input[ctx->pos]) || ctx->input[ctx->pos] == '_')) {
        ctx->pos++;
        while (ctx->pos < ctx->length && (isalnum(ctx->input[ctx->pos]) || ctx->input[ctx->pos] == '_')) {
            ctx->pos++;
        }
        return strndup(ctx->input + start, ctx->pos - start);
    }
    return NULL;
}

char *parse_hex_literal(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    if (ctx->pos + 2 < ctx->length && ctx->input[ctx->pos] == '0' && (ctx->input[ctx->pos+1] == 'x' || ctx->input[ctx->pos+1] == 'X')) {
        ctx->pos += 2;
        size_t digits = 0;
        while (ctx->pos < ctx->length && isxdigit(ctx->input[ctx->pos])) {
            ctx->pos++;
            digits++;
        }
        if (digits > 0) {
            return strndup(ctx->input + start, ctx->pos - start);
        }
        ctx->pos = start;
    }
    return NULL;
}

char *parse_octal_literal(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    if (ctx->pos < ctx->length && ctx->input[ctx->pos] == '0') {
        ctx->pos++;
        size_t digits = 0;
        while (ctx->pos < ctx->length && ctx->input[ctx->pos] >= '0' && ctx->input[ctx->pos] <= '7') {
            ctx->pos++;
            digits++;
        }
        if (digits > 0) {
            return strndup(ctx->input + start, ctx->pos - start);
        }
        ctx->pos = start;
    }
    return NULL;
}

char *parse_binary_literal(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    if (ctx->pos + 2 < ctx->length && ctx->input[ctx->pos] == '0' && (ctx->input[ctx->pos+1] == 'b' || ctx->input[ctx->pos+1] == 'B')) {
        ctx->pos += 2;
        size_t digits = 0;
        while (ctx->pos < ctx->length && (ctx->input[ctx->pos] == '0' || ctx->input[ctx->pos] == '1')) {
            ctx->pos++;
            digits++;
        }
        if (digits > 0) {
            return strndup(ctx->input + start, ctx->pos - start);
        }
        ctx->pos = start;
    }
    return NULL;
}

char *parse_decimal_literal(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    if (ctx->pos < ctx->length && ctx->input[ctx->pos] >= '1' && ctx->input[ctx->pos] <= '9') {
        ctx->pos++;
        while (ctx->pos < ctx->length && isdigit(ctx->input[ctx->pos])) {
            ctx->pos++;
        }
        return strndup(ctx->input + start, ctx->pos - start);
    }
    return NULL;
}

char *parse_float_literal(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    size_t p = ctx->pos;
    int saw_digit = 0, saw_dot = 0, saw_exp = 0;
    if (p < ctx->length && (ctx->input[p] == '+' || ctx->input[p] == '-')) p++;
    while (p < ctx->length && isdigit(ctx->input[p])) { p++; saw_digit = 1; }
    if (p < ctx->length && ctx->input[p] == '.') { p++; saw_dot = 1; }
    while (p < ctx->length && isdigit(ctx->input[p])) { p++; saw_digit = 1; }
    if (p < ctx->length && (ctx->input[p] == 'e' || ctx->input[p] == 'E')) {
        p++;
        saw_exp = 1;
        if (p < ctx->length && (ctx->input[p] == '+' || ctx->input[p] == '-')) p++;
        int exp_digits = 0;
        while (p < ctx->length && isdigit(ctx->input[p])) { p++; exp_digits = 1; }
        if (!exp_digits) return NULL;
    }
    if (p < ctx->length && (ctx->input[p] == 'f' || ctx->input[p] == 'F' || ctx->input[p] == 'l' || ctx->input[p] == 'L')) p++;
    if ((saw_dot || saw_exp) && saw_digit) {
        size_t len = p - start;
        ctx->pos = p;
        return strndup(ctx->input + start, len);
    }
    return NULL;
}

char *parse_char_literal(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    if (ctx->pos < ctx->length && ctx->input[ctx->pos] == '\'') {
        ctx->pos++;
        while (ctx->pos < ctx->length) {
            if (ctx->input[ctx->pos] == '\\') {
                ctx->pos += 2;
            } else if (ctx->input[ctx->pos] == '\'') {
                ctx->pos++;
                return strndup(ctx->input + start, ctx->pos - start);
            } else {
                ctx->pos++;
            }
        }
        ctx->pos = start;
    }
    return NULL;
}

char *parse_string_literal(parser_ctx *ctx) {
    skip_whitespace(ctx);
    size_t start = ctx->pos;
    //if (ctx->pos < ctx->length && ctx->input[ctx->pos] == '@')  ctx->pos++;
    if (ctx->pos < ctx->length && ctx->input[ctx->pos] == '"') {
        ctx->pos++;
        while (ctx->pos < ctx->length) {
            if (ctx->input[ctx->pos] == '\\') {
                ctx->pos += 2;
            } else if (ctx->input[ctx->pos] == '"') {
                ctx->pos++;
                return strndup(ctx->input + start, ctx->pos - start);
            } else {
                ctx->pos++;
            }
        }
        ctx->pos = start;
    }
    return NULL;
}

//////////////////////////////////////////////////////////
//////////////  PARSERS                        ///////////
//////////////////////////////////////////////////////////

statement *parse_statement(parser_ctx *ctx)
{
    expr *e_val = try_parse(0, ctx, expr);
    if (e_val) {
        make_ast(statement, output, {
            .statement_type = is_expr,
            .e_val = e_val
        });
        return output;
    }
    const char *base = ctx->input;
    while (!(e_val = try_parse(0, ctx, expr)) && ctx->input[ctx->pos]) {
        if (ctx->input[ctx->pos + 1] == '}')
            break ;
        ctx->pos += 1;
    }
    if (e_val) {
        make_ast(statement, output, {
            .statement_type = is_expr,
            .e_val = e_val
        })
        return output;
    }
    make_ast(raw, r_val, {
        .source = strndup(base, (ctx->input + ctx->pos) - base)
    });
    make_ast(statement, output, {
        .statement_type = is_raw,
        .r_val = r_val
    })
    return output;
};

compound_statement* parse_compound_statement(parser_ctx *ctx)
{
    if (!try_parse(0, ctx, exact_str,, "{"))
        return NULL;
    try_parse_all(ctx, statement, statements, 0, -1);
    if (!statements_success)
        return NULL;
    if (!try_parse(0, ctx, exact_str,, "}"))
    {
        free(statements);
        return NULL;
    }
    make_ast(compound_statement, output, {
        .statements = (ast**) statements,
        .statements_count = statements_count,
    });
    return output;
}

keyword_arg *parse_keyword_arg(parser_ctx *ctx) {
    char *keyword = NULL;
    char *type = NULL;
    char *name = NULL;

    if (!
        try_parse(&keyword, ctx, identifier)
        && try_parse(0, ctx, exact_str,, ":")
        && try_parse(0, ctx, exact_str,, "(") 
        && try_parse(&type, ctx, type)
        && try_parse(0, ctx, exact_str,, ")")
        && try_parse(&name, ctx, identifier)
    ) {
        free(keyword);
        free(type);
        free(name);
        return NULL;
    }
    make_ast(keyword_arg, output, {
        .keyword = keyword,
        .type = type, 
        .name = name
    });
    return output;
}

method *parse_method_common(parser_ctx *ctx) {
    int method_type;
    char *return_type = NULL;

    if (try_parse(0, ctx, exact_str,, "+")) 
        method_type = static_method;
    else if (try_parse(0, ctx, exact_str,, "-"))
        method_type = member_method;
    else 
        return NULL;
    if (!(try_parse(0, ctx, exact_str,, "(")
        && (return_type = try_parse(0, ctx, type))
        && try_parse(0, ctx, exact_str,, ")")))
    { 
        free(return_type); 
        return NULL; 
    }
    try_parse_all(ctx, keyword_arg, args, 1, -1);
    if (!args_success)
    {
        char *id = try_parse(0, ctx, identifier);
        if (!id)
           return NULL;
        make_ast(keyword_arg, kw, {
            .keyword = id,
            .name = NULL,
            .type = NULL
        })
        args = malloc(sizeof(keyword_arg*));
        *args = kw;
        args_count = 1;
    }
    make_ast(method, var, {
        .method_type = method_type,
        .return_type = return_type,
        .keyword_args = (keyword_arg**) args,
        .keyword_arg_count = args_count,
        .body = NULL
    });
    return var;
}

method *parse_method_proto(parser_ctx *ctx)
{
    method *output = try_parse(0, ctx, method_common);
    if (!output)
        return  NULL;
    if (!try_parse(0, ctx, exact_str,, ";"))
    {
        free(output);
        return NULL;
    }
    return output;
}

method *parse_method_impl(parser_ctx *ctx)
{
    method *output = try_parse(0, ctx, method_common);
    if (!output)
        return  NULL;
    compound_statement * block = try_parse(0, ctx, compound_statement);
    if (!block)
    {
        free(output);
        free(block);
        // TODO: visitor for deletions...
        return NULL;
    }
    output->body = (ast*) block;
    return output;
}

interface *parse_interface(parser_ctx *ctx) {
    if (!try_parse(0, ctx, exact_str,, "@interface"))
        return NULL;
    char *name =  try_parse(0, ctx, identifier);
    if (!name) 
        return NULL;
    char *superclass_name = NULL;
    if (try_parse(0, ctx, exact_str,, ":")) {
        try_parse(&superclass_name, ctx, identifier);
        if (!superclass_name) {
            free(name);
            return NULL;
        }
    }
    compound_statement *ivars = try_parse(0, ctx, compound_statement);
    try_parse_all(ctx, method_proto, methods, 1, -1);
    if (!methods_success) {
        free (name);
        free(superclass_name);
        // todo: delete visitor to delete ivars
        return NULL;
    }
    make_ast(interface, var, {
        .name = name,
        .superclass_name = superclass_name,
        .ivars = ivars,
        .methods = (method**) methods,
        .method_count = methods_count
    });
    return var;
}

implementation *parse_implementation(parser_ctx *ctx) {
    if (!try_parse(0, ctx, exact_str,, "@implementation"))
        return NULL;
    char *name =  try_parse(0, ctx, identifier);
    if (!name) 
        return NULL;
    
    try_parse_all(ctx, method_impl, methods, 1, -1);
    if (!methods_success) {
        free (name);
        return NULL;
    }
    make_ast(implementation, var, {
        .name = name,
        .methods = (method**) methods,
        .method_count = methods_count
    });
    return var;
}

message_param *parse_message_param(parser_ctx *ctx)
{
    char *keyword = try_parse(0, ctx, identifier);
    if (!keyword)
        return NULL;
    if (!try_parse(0, ctx, exact_str,, ":"))
    {
        free(keyword);
        return NULL;
    }
    expr *e = try_parse(0, ctx, expr);
    if (!e) {
        free(keyword);
        return NULL;
    }
    make_ast(message_param, output, {
        .keyword = keyword,
        .value = e
    });
    return output;
}

message *parse_message(parser_ctx *ctx) {
    if (!try_parse(0, ctx, exact_str,, "[")) 
        return NULL;
    expr *receiver = try_parse(0, ctx, expr);
    if (!receiver) 
        return NULL;

    char *sel1 = NULL;
    try_parse_all(ctx, message_param, params, 1, -1);
    if (!params_success) {
        try_parse(&sel1, ctx, identifier);
        if (!sel1)
        {
            free(receiver);
            return NULL;
        }
        make_ast(message_param, param, {
            .keyword = sel1,
            .value = NULL
        })
        params = malloc(sizeof(message_param*));
        *params = param;
        params_count = 1;
    }
    if (!try_parse(0, ctx, exact_str,, "]")) 
    { 
        free(receiver);
        free(sel1);
        // todo: free all params using delete visitor
        return NULL; 
    }
    make_ast(message, var, {
        .receiver = receiver,
        .params = (message_param**) params,
        .params_count = params_count
    });
    return var;
}

top_level *parse_top_level(parser_ctx *ctx) {
    make_ast(top_level, output, {.childs = malloc(sizeof(void*)), .size = 0});
    const char *start_raw = NULL;
    while (ctx->pos < ctx->length) {
        ast *node = NULL;
        node = NULL;
        if (try_parse(&node, ctx, interface) || try_parse(&node, ctx, implementation) || try_parse(&node, ctx, expr)) {
            if (start_raw)
            {
                make_ast(raw, r_node, {
                    .source = strndup(start_raw, ctx->input + ctx->pos - start_raw)
                });
                output->size += 1;
                output->childs = realloc(output->childs, output->size * sizeof(ast*));
                output->childs[output->size] = (ast*) r_node;
            }
            output->size += 1;
            output->childs = realloc(output->childs, output->size * sizeof(ast*));
            output->childs[output->size] = node;
            start_raw = NULL;
        } else {
            if (!start_raw)
                start_raw = ctx->input;
            ctx->pos += 1;
        }
    }
    if (start_raw) {
        make_ast(raw, r_node, {
            .source = strndup(start_raw, ctx->input + ctx->pos - start_raw)
        });
        output->size += 1;
        output->childs = realloc(output->childs, output->size * sizeof(ast*));
        output->childs[output->size] = (ast*) r_node;
    }
    return output;
}

// Top-level expression parser
expr *parse_expr(parser_ctx *ctx) {
    return parse_assignment_expr(ctx);
}

expr *parse_assignment_expr(parser_ctx *ctx) {
    // TODO: implement assignment parsing
    return parse_conditional_expr(ctx);
}

expr *parse_conditional_expr(parser_ctx *ctx) {
    // TODO: implement conditional parsing
    return parse_logical_or_expr(ctx);
}

expr *parse_logical_or_expr(parser_ctx *ctx) {
    // TODO: implement logical or parsing
    return parse_logical_and_expr(ctx);
}

expr *parse_logical_and_expr(parser_ctx *ctx) {
    // TODO: implement logical and parsing
    return parse_inclusive_or_expr(ctx);
}

expr *parse_inclusive_or_expr(parser_ctx *ctx) {
    // TODO: implement inclusive or parsing
    return parse_exclusive_or_expr(ctx);
}

expr *parse_exclusive_or_expr(parser_ctx *ctx) {
    // TODO: implement exclusive or parsing
    return parse_and_expr(ctx);
}

expr *parse_and_expr(parser_ctx *ctx) {
    // TODO: implement and parsing
    return parse_equality_expr(ctx);
}

expr *parse_equality_expr(parser_ctx *ctx) {
    // TODO: implement equality parsing
    return parse_relational_expr(ctx);
}

expr *parse_relational_expr(parser_ctx *ctx) {
    // TODO: implement relational parsing
    return parse_shift_expr(ctx);
}

expr *parse_shift_expr(parser_ctx *ctx) {
    // TODO: implement shift parsing
    return parse_additive_expr(ctx);
}

expr *parse_additive_expr(parser_ctx *ctx) {
    expr *left = parse_multiplicative_expr(ctx);
    if (!left) return NULL;
    while (1) {
        skip_whitespace(ctx);
        char op = 0;
        if (ctx->input[ctx->pos] == '+') op = '+';
        else if (ctx->input[ctx->pos] == '-') op = '-';
        else break;
        ctx->pos++;
        expr *right = parse_multiplicative_expr(ctx);
        if (!right) break;
        make_ast(binop_expr, bin, {.left = (ast*)left, .op = (char[]){op, 0}, .right = (ast*)right});
        left = (expr*)bin;
    }
    return left;
}

expr *parse_multiplicative_expr(parser_ctx *ctx) {
    expr *left = parse_unary_expr(ctx);
    if (!left) return NULL;
    while (1) {
        skip_whitespace(ctx);
        char op = 0;
        if (ctx->input[ctx->pos] == '*') op = '*';
        else if (ctx->input[ctx->pos] == '/') op = '/';
        else if (ctx->input[ctx->pos] == '%') op = '%';
        else break;
        ctx->pos++;
        expr *right = parse_unary_expr(ctx);
        if (!right) break;
        make_ast(binop_expr, bin, {.left = (ast*)left, .op = (char[]){op, 0}, .right = (ast*)right});
        left = (expr*)bin;
    }
    return left;
}

expr *parse_unary_expr(parser_ctx *ctx) {
    // TODO: implement unary parsing
    return parse_postfix_expr(ctx);
}

expr *parse_postfix_expr(parser_ctx *ctx) {
    // TODO: implement postfix parsing
    return parse_primary_expr(ctx);
}

expr *parse_primary_expr(parser_ctx *ctx) {
    char *id = NULL;
    try_parse(&id, ctx, identifier);
    if (id) {
        make_ast(raw, rawnode, {.source = id});
        ast **children = malloc(sizeof(ast*));
        children[0] = (ast*)rawnode;
        make_ast(expr, var, {.children = children, .child_count = 1});
        return var;
    }
    char *num = NULL;
    try_parse(&num, ctx, decimal_literal);
    if (num) {
        make_ast(raw, rawnode, {.source = num});
        ast **children = malloc(sizeof(ast*));
        children[0] = (ast*)rawnode;
        make_ast(expr, var, {.children = children, .child_count = 1});
        return var;
    }
    return NULL;
}