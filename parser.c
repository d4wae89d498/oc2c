#include "ast.h"
#include "parser.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include "./_strdup.h"
#include "libobjc/include/objc/objc.h"
#include "visitors/deleter.h"
#include "visitors/dumper.h"


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
    };
}

static void restore_ctx_state(parser_ctx *ctx, parser_ctx_state state) {
    ctx->pos = state.pos;
}

//////////////////////////////////////////////////////////
//////////////  HELPERS                        ///////////
//////////////////////////////////////////////////////////

static int skip_comments(parser_ctx *ctx) {
    while (1) {
        while (isspace(ctx->input[ctx->pos]))
            ctx->pos++;
        if (ctx->input[ctx->pos] == '/' && ctx->input[ctx->pos + 1] == '/') {
            ctx->pos += 2;
            while (ctx->input[ctx->pos] && ctx->input[ctx->pos] != '\n')
                ctx->pos++;
            continue;
        }
        if (ctx->input[ctx->pos] == '/' && ctx->input[ctx->pos + 1] == '*') {
            ctx->pos += 2;
            while (ctx->input[ctx->pos] && !(ctx->input[ctx->pos] == '*' && ctx->input[ctx->pos + 1] == '/')) {
                ctx->pos++;
            }
            if (ctx->input[ctx->pos] == '*' && ctx->input[ctx->pos + 1] == '/')
                ctx->pos += 2;
            else
                break;
            continue;
        }
        break;
    }
    return 1;
}


//static void parse_skip_whitespace(parser_ctx *ctx) {
//    return skip_whitespace(ctx);
//}

//////

static char *exact_str(parser_ctx *ctx, char *what) {
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

int find_closing_paren_len(const char *s) {
    if (!s || s[0] != '(') return -1;  // Must start with '('

    int paren_level = 1;    // We start inside one '('
    int bracket_level = 0;  // Track nesting of '[' and ']'

    for (int i = 1; s[i] != '\0'; i++) {
        char c = s[i];

        if (c == '{' || c == '}') {
            // Error on curly braces
            return -1;
        }

        if (c == '[') {
            bracket_level++;
        } else if (c == ']') {
            if (bracket_level == 0) {
                // Unmatched closing bracket, treat as error or ignore?
                // Here, let's treat as error:
                return -1;
            }
            bracket_level--;
        } else if (c == '(') {
            if (bracket_level == 0) {
                paren_level++;
            }
            // If inside brackets, '(' is just a char, no paren count
        } else if (c == ')') {
            if (bracket_level == 0) {
                paren_level--;
                if (paren_level == 0) {
                    // Found the matching closing ')', return length including it
                    return i + 1;
                }
            }
            // If inside brackets, ')' is just a char
        }
        // Other chars are ignored, just keep scanning
    }

    // If we finish the loop and paren_level != 0, no matching closing paren found
    return -1;
}


//////////////////////////////////////////////////////////
//////////////  BACKTRACKING                   ///////////
//////////////////////////////////////////////////////////

static void* do2(int a, void *b)
{
    (void) a;
    return b;
}

// --- try_parse macro and helper ---
#define try_parse(ptr, ctx, parser, ...) \
    _try_parse(#parser, ctx, save_ctx_state(ctx), do2(skip_comments(ctx), parse_ ## parser(ctx __VA_ARGS__)), (void**)ptr)

static void *_try_parse(char *name, parser_ctx *ctx, parser_ctx_state state, void *result, void **ptr) {
    if (!result) {
        printf("----- [%zu] Parsing FAILED for %s at [%.12s]..\n", state.pos, name, ctx->input + state.pos);
        restore_ctx_state(ctx, state);
    } else {
        printf("[%zu] Parsing ok for %s at [%.*s]..\n", state.pos, name, (int)(ctx->pos - state.pos), ctx->input + state.pos);
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
            args ## _count += 1;                              \
            args = realloc(args, args ## _count * sizeof(void*)); \
            args[args ## _count - 1] = current;               \
        }                                                     \
    }                                                         \
    if (args ## _count >= min &&  (max == -1 || args ## _count <= max))     \
        args ## _success = 1;                                 \
    else                                                      \
        restore_ctx_state(ctx, state);



//////////////////////////////////////////////////////////
//////////////  LEXERS                         ///////////
//////////////////////////////////////////////////////////

char *parse_identifier(parser_ctx *ctx) {
    size_t start = ctx->pos;
    if (ctx->pos < ctx->length && (isalpha(ctx->input[ctx->pos]) || ctx->input[ctx->pos] == '_')) {
        ctx->pos++;
        while (ctx->pos < ctx->length && (isalnum(ctx->input[ctx->pos]) || ctx->input[ctx->pos] == '_')) {
            ctx->pos++;
        }
       return strndup(ctx->input + start, ctx->pos - start) ;
    }
    return NULL;
}

raw *parse_identifier_raw(parser_ctx *ctx) {
    char *s = parse_identifier(ctx);
    if (!s)
        return NULL;
    make_ast(raw, output, {
        .source = s
    });
    return output;
}

char *parse_hex_literal(parser_ctx *ctx) {
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
    size_t start = ctx->pos;
    //if (ctx->pos < ctx->length && ctx->input[ctx->pos] == '@')  ctx->pos++;
    if (ctx->pos < ctx->length && ctx->input[ctx->pos] == '"') {
        ctx->pos++;
        while (ctx->pos < ctx->length) {
            if (ctx->input[ctx->pos] == '\\') {
                ctx->pos += 1; // todo:: check how many //
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


char *parse_paren_type(parser_ctx *ctx) {
    int closing = find_closing_paren_len(ctx->input + ctx->pos);
    if (closing == -1) 
        return NULL;
    char *output = strndup(ctx->input + ctx->pos + 1, closing - 2);
    ctx->pos += closing;
    return (char*) output;
}

//////////////////////////////////////////////////////////
//////////////  PARSERS                        ///////////
//////////////////////////////////////////////////////////


encode *parse_encode(parser_ctx *ctx)
{
    char *ostr = NULL;
    if (try_parse(0, ctx, exact_str,, "@encode")
    && try_parse(&ostr, ctx, paren_type))
    {
        make_ast(encode, onode, {
            .type = ostr
        })
        return onode;
    }
    return NULL;
}

selector *parse_selector(parser_ctx *ctx)
{
    try_parse(0, ctx, exact_str,, "@selector");
    
    try_parse(0, ctx, exact_str,, "(");
    char *output = malloc(1);
    *output = 0;
    char *kw;
    if (!try_parse(&kw, ctx, identifier))
        return NULL;
    output = realloc(output, strlen(kw) + 2);
    strcpy(output, kw);
    if (try_parse(0, ctx, exact_str,, ":"))
    {
        strcat(output, ":");
        while (1) {
            if (try_parse(&kw, ctx, identifier) 
                && try_parse(0, ctx, exact_str,, ":"))
            {
                output = realloc(output, strlen(output) + strlen(kw) + 1);
                strcat(output, kw);       
            }
            else 
                break;
        }
    }
    if (!try_parse(0, ctx, exact_str,, ")"))
        return NULL;

    make_ast(selector, output_expr, {
        .str = _strdup(output)
    });
    return output_expr;
}

keyword_arg *parse_keyword_arg(parser_ctx *ctx) {
    char *keyword = NULL;
    char *type = NULL;
    char *name = NULL;

    if (!
        (try_parse(&keyword, ctx, identifier)
        && try_parse(0, ctx, exact_str,, ":")
        && try_parse(&type, ctx, paren_type)
        && try_parse(&name, ctx, identifier))
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
    return_type = try_parse(0, ctx, paren_type);
    if (!return_type)
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
        deleter_visitor.method(output, output);
        deleter_visitor.compound_statement(block, ctx);
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
    printf("begin.\n");
    compound_statement *ivars = try_parse(0, ctx, compound_statement);
    printf("%p.\n", ivars);

    try_parse_all(ctx, method_proto, methods, 0, -1);
    printf("???");
    if (!methods_success) {
        free (name);
        free(superclass_name);
        // todo: delete visitor to delete ivars
        return NULL;
    }
    printf("ok!");
    if (!try_parse(0, ctx, exact_str,, "@end"))
        return NULL;
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
    if (!try_parse(0, ctx, exact_str,, "@end"))
    {
        // todo:: free if an issue using deleter_visitor
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
    make_ast(top_level, output, {
        .childs = malloc(sizeof(void*)), 
        .size   = 0
    });

    const char *start_raw = NULL;

    while (ctx->pos < ctx->length) {
        ast     *node        = NULL;
        size_t   parse_start = ctx->pos;           // ← remember where parsing would begin

        if ( try_parse(&node, ctx, interface)
          || try_parse(&node, ctx, implementation)
          || try_parse(&node, ctx, compound_statement)
        ) {
            // 1) first, flush any raw text *up to* parse_start, not ctx->pos
            if (start_raw) {
                size_t raw_len = (start_raw - ctx->input)
                               ? parse_start - (start_raw - ctx->input)
                               : parse_start - (start_raw - ctx->input);
                make_ast(raw, r_node, {
                    .source = strndup(start_raw, raw_len)
                });
                output->childs = realloc(output->childs,
                                         (output->size + 1) * sizeof(ast*));
                output->childs[output->size++] = (ast*)r_node;
                start_raw = NULL;

            }

            // 2) then insert the just‑parsed AST node
            output->childs = realloc(output->childs,
                                     (output->size + 1) * sizeof(ast*));
            output->childs[output->size++] = node;


        }
        else {
            // we’re in a raw zone; mark its start if needed
            if (!start_raw)
                start_raw = ctx->input + ctx->pos;
            ctx->pos += 1;
        }
    }

    // flush any trailing raw text
    if (start_raw) {
        size_t raw_len = ctx->input + ctx->pos - start_raw;
        make_ast(raw, r_node, {
            .source = strndup(start_raw, raw_len)
        });
        output->childs = realloc(output->childs,
                                 (output->size + 1) * sizeof(ast*));
        output->childs[output->size++] = (ast*)r_node;
    }

    return output;
}

statement *parse_statement(parser_ctx *ctx)
{
    if (try_parse(0, ctx, exact_str,, "{")) {
        ctx->pos -= 1;
        make_ast(statement, output, {
            .statement_type = is_cp,
            .cp_val = parse_compound_statement(ctx)
        })
        return output;
    }
    expr *e_val = try_parse(0, ctx, expr);
    if (e_val && try_parse(0, ctx, exact_str,, ";")) {
        make_ast(statement, output, {
            .statement_type = is_expr,
            .e_val = e_val
        });
        return output;
    }
    return NULL;
};

static inline void    cp_flush(parser_ctx *ctx, compound_statement *pc, const char **ptr, size_t end_pos)
{
    if (!*ptr || ctx->pos >= ctx->length)
        return;
    make_ast(raw, rnode, {
        .source = _strndup(*ptr, ctx->input + end_pos - *ptr)
    })
    make_ast(statement, output, {
        .statement_type = is_raw,
        .r_val = rnode
    })
    pc->statements_count += 1;
    pc->statements = realloc(pc->statements, pc->statements_count * sizeof(void*));
    pc->statements[pc->statements_count - 1] = output;
    *ptr = NULL;
}

compound_statement* parse_compound_statement(parser_ctx *ctx)
{
    printf(".. [%.10s]!\n", ctx->input + ctx->pos);
    if (!try_parse(0, ctx, exact_str,, "{"))
        return NULL;
    make_ast(compound_statement, output, {
        .statements = (void*) malloc(sizeof(void*)),
        .statements_count = 0,
    });
    const char *start = NULL;
    while (ctx->input[ctx->pos])
    {
        statement *stmt;
        size_t end_pos = ctx->pos;
        if (try_parse(&stmt, ctx, statement))
        {
             cp_flush(ctx, output, &start, end_pos);
             output->statements_count += 1;
             output->statements = realloc(output->statements, output->statements_count * sizeof(void*));
             output->statements[output->statements_count - 1] = (void*) stmt;
             
        } else if (try_parse(0, ctx, exact_str,, "}")) {
            cp_flush(ctx, output, &start, end_pos);
            return output;
        } else {
            if (!start)
                start = ctx->input + ctx->pos;
            ctx->pos += 1;
        }
    }
    cp_flush(ctx, output, &start, ctx->pos);
    return output;
}

ast *parse_literal(parser_ctx *ctx) {
    char *lit;
    printf("b4< %zu\n", ctx->pos);
    try_parse(&lit, ctx, hex_literal)
    || try_parse(&lit, ctx, octal_literal)
    || try_parse(&lit, ctx, binary_literal)
    || try_parse(&lit, ctx, float_literal)
    || try_parse(&lit, ctx, decimal_literal)
    || try_parse(&lit, ctx, char_literal)
    || try_parse(&lit, ctx, string_literal) ;
    if (!lit)
        return NULL;
    make_ast(raw, output, {
        .source = lit
    });
    printf("after< %zu\n", ctx->pos);
    printf("parse ok :: %s\n", lit);
    return (ast*)output;
}

//// C EXPR

/*
castExpression
    : (LP typeName RP) castExpression
    | unaryExpression
    ;
*/

ast *parse_cast_expr(parser_ctx *ctx) {
    
    char *ptype = try_parse(0, ctx, paren_type);
    if (!ptype)
        return parse_unary_expr(ctx);
    make_ast(cast_expr, output, {
        .type = ptype,
        .expr = try_parse(0, ctx, cast_expr)
    })
    if (ptype && output->expr)
        return (ast*) output;
    else 
        return NULL;
   

};

///////////////////////

# define declare_left_reccursive_rule(name, operands_parser)\
ast *parse_ ## name (parser_ctx *ctx)                   \
{                                                       \
    ast *left = NULL;                                   \
    ast *right = NULL;                                  \
    if (!try_parse(&left, ctx, operands_parser))        \
        return NULL;                                    \
    while (1)                                           \
    {                                                   \
        char *op = NULL;                                \
        if (!try_parse(&op, ctx, name ## _op))          \
        {                                               \
            break;                                      \
        }                                               \
        if (!try_parse(&right, ctx, operands_parser))   \
            return NULL;                                \
        make_ast(binop_expr, bin, {                     \
            .op = op,                                   \
            .left = left,                               \
            .right = right                              \
        });                                             \
        left = (ast*) bin;                              \
    }                                                   \
    return left;                                        \
}

///////////////////////

/***********************************************************************
multiplicativeExpression
    : castExpression (('*' | '/' | '%') castExpression)*
    ;

*/

char *parse_mult_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "*")
    || try_parse(&op, ctx, exact_str,, "/")
    || try_parse(&op, ctx, exact_str,, "%"));
    return op;
}
declare_left_reccursive_rule(mult_expr, cast_expr)

/***********************************************************************
additiveExpression
    : multiplicativeExpression (('+' | '-') multiplicativeExpression)*
    ;

*/
char *parse_add_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "+")
    || try_parse(&op, ctx, exact_str,, "-"));
    return op;
}
declare_left_reccursive_rule(add_expr, mult_expr)

/***********************************************************************
shiftExpression
    : additiveExpression ((leftShiftOperator | rightShiftOperator) additiveExpression)*
    ;

leftShiftOperator
    : LT LT
    ;
    
rightShiftOperator
    : GT GT
    ;
*/

char *parse_shift_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "<<")
    || try_parse(&op, ctx, exact_str,, ">>"));
    return op;
}
declare_left_reccursive_rule(shift_expr, add_expr)

/***********************************************************************
relationalExpression
    : shiftExpression ((LT | GT | LE | GE) shiftExpression)*
    ;

*/
char *parse_relational_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "<")
    || try_parse(&op, ctx, exact_str,, ">")
    || try_parse(&op, ctx, exact_str,, "<=")
    || try_parse(&op, ctx, exact_str,, ">="));
    return op;
}
declare_left_reccursive_rule(relational_expr, shift_expr)

/***********************************************************************
equalityExpression
    : relationalExpression ((EQUAL | NOTEQUAL) relationalExpression)*
    ;
*/

char *parse_equality_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "==")
    || try_parse(&op, ctx, exact_str,, "!="));
    return op;
}
declare_left_reccursive_rule(equality_expr, relational_expr)

/***********************************************************************
andExpression
    : equalityExpression (BITAND equalityExpression)*
    ;
*/
char *parse_and_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "&"));
    return op;
}
declare_left_reccursive_rule(and_expr, equality_expr)

/***********************************************************************
exclusiveOrExpression
    : andExpression (BITXOR andExpression)*
    ;
*/
char *parse_xor_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "^"));
    return op;
}
declare_left_reccursive_rule(xor_expr, and_expr)

/***********************************************************************
inclusiveOrExpression
    : exclusiveOrExpression (BITOR exclusiveOrExpression)*
    ;
*/
char *parse_or_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "|"));
    return op;
}
declare_left_reccursive_rule(or_expr, xor_expr)

/***********************************************************************
logicalAndExpression
    : inclusiveOrExpression (AND inclusiveOrExpression)*
    ;
*/
char *parse_logical_and_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "&&"));
    return op;
}
declare_left_reccursive_rule(logical_and_expr, or_expr)

/***********************************************************************
logicalOrExpression
    : logicalAndExpression (OR logicalAndExpression)*
    ;
*/
char *parse_logical_or_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    (try_parse(&op, ctx, exact_str,, "||"));
    return op;
}
declare_left_reccursive_rule(logical_or_expr, logical_and_expr)

/***********************************************************************
conditionalExpression
    : logicalOrExpression ('?' ifExpr = conditionalExpression ':' elseExpr = conditionalExpression)?
    ;
*/
ast *parse_conditional_expr(parser_ctx *ctx) {
    ast *e = try_parse(0, ctx, logical_or_expr);
    if (!try_parse(0, ctx, exact_str,, "?"))
        return e;
    ast *ifExpr = try_parse(0, ctx, conditional_expr);
    if (!ifExpr)
        return NULL;
    if (!try_parse(0, ctx, exact_str,, ":"))
        return NULL;
    ast *elseExpr = try_parse(0, ctx, conditional_expr);
    if (!elseExpr)
        return NULL;
    make_ast(conditional_expr, output, {
        .test = e,
        .consequent = ifExpr,
        .alternate = elseExpr
    });
    return (ast*) output;
}

/***********************************************************************
assignmentExpression
    : conditionalExpression
    | unaryExpression assignmentOperator assignmentExpression
    ;
*/
char *parse_assignment_expr_op(parser_ctx *ctx);

ast *parse_assignment_expr(parser_ctx *ctx)
{
    parser_ctx_state ctx_bkp = save_ctx_state(ctx);
    ast *left = try_parse(0, ctx, unary_expr);
    if (left) {
        char *op = try_parse(0, ctx, assignment_expr_op);
        if (op) {
            ast *right = try_parse(0, ctx, assignment_expr);
            if (right) {
                make_ast(binop_expr, output, {
                    .op    = op,
                    .left  = left,
                    .right = right
                });
                return (ast *)output;
            }
            return NULL;
        }
        restore_ctx_state(ctx, ctx_bkp);
    }
    return try_parse(0, ctx, conditional_expr);
}

/***********************************************************************
assignmentOperator
    : '='
    | '*='
    | '/='
    | '%='
    | '+='
    | '-='
    | '<<='
    | '>>='
    | '&='
    | '^='
    | '|='
    ;
*/

char *parse_assignment_expr_op(parser_ctx *ctx) {
    char *op = NULL;
    try_parse(&op, ctx, exact_str,, "=")
    || try_parse(&op, ctx, exact_str,, "*=")
    || try_parse(&op, ctx, exact_str,, "/=")
    || try_parse(&op, ctx, exact_str,, "%=")
    || try_parse(&op, ctx, exact_str,, "+=")
    || try_parse(&op, ctx, exact_str,, "-=")
    || try_parse(&op, ctx, exact_str,, "<<=")
    || try_parse(&op, ctx, exact_str,, ">>=")
    || try_parse(&op, ctx, exact_str,, "&=")
    || try_parse(&op, ctx, exact_str,, "^=")
    || try_parse(&op, ctx, exact_str,, "|=")
    ;
    return op;
}

/***********************************************************************
expression
    : assignmentExpression (',' assignmentExpression)*
    ;
*/
expr *parse_expr(parser_ctx *ctx) {
    make_ast(expr, output, {
        .exprs_count = 0
    });
    ast *binop = try_parse(0, ctx, assignment_expr);
    if (!binop)
        return NULL;
    output->exprs_count = 1;
    output->exprs = malloc(sizeof(void*));
    *(output->exprs) = (ast*)binop;
    while (try_parse(0, ctx, exact_str,, ",")) {
        binop = try_parse(0, ctx, assignment_expr);
        if (!binop)
            break;
        output->exprs = realloc(output->exprs, (output->exprs_count + 1) * sizeof(void*));
        output->exprs[output->exprs_count] = binop;
        output->exprs_count += 1;
    }
    return output;
}


/***********************************************************************
unaryExpression
    : ('++' | '--')* (
        postfixExpression
        | unaryOperator castExpression
        | ('sizeof' | '_Alignof') LP typeName RP
    )
    ;

unaryOperator
    : '&'
    | '*'
    | '+'
    | '-'
    | '~'
    | BANG
    ;
*/

ast *parse_unary_expr(parser_ctx *ctx) {
    char *op = NULL;
    if (try_parse(&op, ctx, exact_str,, "++")
        || try_parse(&op, ctx, exact_str,, "--"))
    {
        make_ast(unary_op_expr, new_output, {
            .arg = NULL,
            .expr = NULL
        });
        new_output->op = op;
        new_output->expr = parse_unary_expr(ctx);
        if (!new_output->expr)
            return NULL;
        new_output->pos = unary_op_expr_prefix;
        return (ast*)new_output;
    }
    printf("????\n");
    unary_op_expr *pf = try_parse(0, ctx, postfix_expr);
    if (pf)
    {
        printf("ue ok\n");
        return (ast*) pf;
    }

    try_parse(&op, ctx, exact_str,, "&")
    ||  try_parse(&op, ctx, exact_str,, "*")
    ||  try_parse(&op, ctx, exact_str,, "+")
    ||  try_parse(&op, ctx, exact_str,, "-")
    ||  try_parse(&op, ctx, exact_str,, "~")
    ||  try_parse(&op, ctx, exact_str,, "!");
    if (op) {
        make_ast(unary_op_expr, new_output, {
            .arg = NULL
        });
        new_output->op = op;
        new_output->expr = parse_cast_expr(ctx);
        if (!new_output->expr)
            return NULL;
        new_output->pos = unary_op_expr_prefix;
        return (ast*)new_output;
    }

    try_parse(&op, ctx, exact_str,, "sizeof")
    || try_parse(&op, ctx, exact_str,, "_Alignof");
    if (op) {
        make_ast(unary_op_expr, new_output, {
            .expr = NULL,
            .arg = NULL
        });
        new_output->op = op;
        new_output->pos = unary_op_expr_prefix;
        make_ast(raw, raw_output, {
            .source = parse_paren_type(ctx)
        });
        if (!raw_output->source)
            return NULL;
        new_output->arg = (ast*)raw_output;
        return (ast*)new_output;
    }

    return NULL;
}



/***********************************************************************
postfixExpression
    : (primaryExpression | LP typeName RP '{' initializerList ','? '}') (
        '[' expression ']'
        | LP argumentExpressionList? RP
        | ('.' | '->') identifier
        | '++'
        | '--'
    )*
    ;
*/

ast *parse_postfix_expr(parser_ctx *ctx) {
    ast *expr = try_parse(0, ctx, primary_expr);
    if (expr)
    {
        printf("pf expr ok\n");
    //    return expr;
    }
  //  return NULL;
    char *op = NULL;
    while (1) {
        if (try_parse(&op, ctx, exact_str,, "[") 
            || try_parse(&op, ctx, exact_str,, "(")) {
            ast *arg = try_parse(0, ctx, expr);
            printf("-======s------ [%p]::\n", arg);
            //dumper_visitor.expr(arg, 0);
            make_ast(unary_op_expr, new_expr, {
                .op = op,
                .expr = expr,
                .pos = unary_op_expr_sufix,
                .arg = arg
            });
            arg->accept(arg, dumper_visitor, 0);
            printf("--\n");
            new_expr->base.accept((ast*)new_expr, dumper_visitor, 0);
            printf("--\n");
            if (!strcmp(op, "["))
            {
                if (!try_parse(0, ctx, exact_str,, "]"))
                    return NULL;
            }
            else 
                if (!try_parse(0, ctx, exact_str,, ")"))
                    return NULL;
            expr = (ast*) new_expr;
        } else if (try_parse(&op, ctx, exact_str,, ".")
        || try_parse(&op, ctx, exact_str,, "->")) {
            ast *id = try_parse(0, ctx, identifier_raw);
            make_ast(unary_op_expr, new_expr, {
                .op = op,
                .expr = expr,
                .pos = unary_op_expr_sufix,
                .arg = id
            });      
            expr = (ast*) new_expr;
        } else if (try_parse(&op, ctx, exact_str,, "++")
            || try_parse(&op, ctx, exact_str,, "--")) {
            make_ast(unary_op_expr, new_expr, {
                .op = op,
                .expr = expr,
                .pos = unary_op_expr_sufix,
                .arg = NULL
            });
            expr = (ast*) new_expr;
        } else {
            break ;
        }
    }
    return expr;
}



/***********************************************************************
primaryExpression
    : identifier
    | constant
    | stringLiteral
    | LP expression RP
    | messageExpression
    | selectorExpression
    | protocolExpression                                REMOVED
    | encodeExpression
    | dictionaryLiteralExpression                       REMOVED
    | arrayLiteralExpression                            REMOVED
    | boxedExpression                                   REMOVED
    | blockExpression                                   REMOVED
    | '__extension__'? LP compoundStatement RP          REMOVED
    ;
*/

ast *parse_pexpr(parser_ctx *ctx)
{
    ast *node = NULL;

    if (
        try_parse(0, ctx, exact_str,, "(")
    &&  try_parse(&node, ctx, expr)
    &&  try_parse(0, ctx, exact_str,, ")"))

    {
        return node;
    }
    return NULL;
    
}

ast *parse_primary_expr(parser_ctx *ctx)
{
    ast *output;
    try_parse(&output, ctx, identifier_raw)
    || try_parse(&output, ctx, literal)
    || try_parse(&output, ctx, pexpr)
    || try_parse(&output, ctx, message)
    || try_parse(&output, ctx, encode)
    || try_parse(&output, ctx, selector);
    printf("parse ok pe\n");
    return output; 
}