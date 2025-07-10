#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

// Parser context definition
struct parser_ctx {
    const char *input;
    size_t pos;
    size_t length;
    size_t depth;
};

typedef struct parser_ctx parser_ctx;
typedef struct parser_ctx_state parser_ctx_state; 

// LEXERS:
char *parse_identifier(parser_ctx *ctx);
raw *parse_identifier_raw(parser_ctx *ctx);

char *parse_hex_literal(parser_ctx *ctx);
char *parse_octal_literal(parser_ctx *ctx);
char *parse_binary_literal(parser_ctx *ctx);
char *parse_decimal_literal(parser_ctx *ctx);
char *parse_float_literal(parser_ctx *ctx);
char *parse_char_literal(parser_ctx *ctx);
char *parse_string_literal(parser_ctx *ctx);

// OJB-C
method *parse_method(parser_ctx *ctx);
keyword_arg *parse_keyword_arg(parser_ctx *ctx);
interface *parse_interface(parser_ctx *ctx);
implementation *parse_implementation(parser_ctx *ctx);
message *parse_message(parser_ctx *ctx);


// C
top_level *parse_top_level(parser_ctx *ctx);
raw *parse_raw(parser_ctx *ctx);
expr *parse_type(parser_ctx *ctx);
statement *parse_statement(parser_ctx *ctx);
compound_statement *parse_compound_statement(parser_ctx *ctx);
message_param *parse_message_param(parser_ctx *ctx);
expr *parse_expr(parser_ctx *ctx);
ast *parse_conditional_expr(parser_ctx *ctx);
ast *parse_assignment_expr(parser_ctx *ctx);
ast *parse_assignement_expr(parser_ctx *ctx);
ast *parse_unary_expr(parser_ctx *ctx);
ast *parse_postfix_expr(parser_ctx *ctx);
ast *parse_primary_expr(parser_ctx *ctx);

#endif // PARSER_H
