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

char *parse_ivar(parser_ctx *ctx);
param_node *parse_param(parser_ctx *ctx);
method_node *parse_method(parser_ctx *ctx);
interface_node *parse_interface(parser_ctx *ctx);
implementation_node *parse_implementation(parser_ctx *ctx);
message_node *parse_message(parser_ctx *ctx);
selector_node *parse_selector(parser_ctx *ctx);
raw_node *parse_raw(parser_ctx *ctx);
tu_node *parse_tu(parser_ctx *ctx);
expr_node *parse_expr(parser_ctx *ctx);

char *parse_str(parser_ctx *ctx, char *what);
char *parse_identifier(parser_ctx *ctx);

#endif // PARSER_H
