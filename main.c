#include "./parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "visitors/dumper.h"
#include "visitors/transpiler.h"
#include "visitors/c_transpiler.h"
#include <string.h>
#include "_fd_copy.h"

void print_help(const char *prog) {
    printf("Usage: %s [--visitor dumper|transpiler] <file.m>\n", prog);
    printf("  --visitor dumper      Print AST\n");
    printf("  --visitor transpiler  Output C (default)\n");
    printf("  -h, --help            Show this help\n");
}

static int parse_args(int ac, char **av, const char **filename, const char **visitor) {
    *filename = NULL;
    *visitor = "transpiler";
    for (int i = 1; i < ac; ++i) {
        if (strcmp(av[i], "-h") == 0 || strcmp(av[i], "--help") == 0) {
            print_help(av[0]);
            return 1;
        } else if (strcmp(av[i], "--visitor") == 0 && i+1 < ac) {
            *visitor = av[++i];
        } else if (!*filename) {
            *filename = av[i];
        } else {
            fprintf(stderr, "Unknown argument: %s\n", av[i]);
            print_help(av[0]);
            return 1;
        }
    }
    if (!*filename) {
        print_help(av[0]);
        return 1;
    }
    return 0;
}

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(sz+1);
    if (!buf) { fclose(f); return NULL; }
    if (fread(buf, 1, sz, f) != (size_t)sz) { fclose(f); free(buf); return NULL; }
    buf[sz] = 0;
    fclose(f);
    return buf;
}

static void run_visitor(const char *visitor, top_level *root) {
    if (strcmp(visitor, "dumper") == 0) {
        dumper_visitor.top_level(root, 0);
    } else if (strcmp(visitor, "transpiler") == 0) {
        transpiler_visitor.top_level(root, NULL);
    } else if (strcmp(visitor, "c_transpiler") == 0) {

        printf("running...\n");
        char *target_name = "a";

        c_transpiler_ctx ctx = c_transpiler_ctx_init();
        fprintf(ctx.iface, "#ifndef ___%s_h\n", target_name);
        fprintf(ctx.iface, "# define ___%s_h\n", target_name);
        fprintf(ctx.iface, "# include <objc/runtime.h>\n");
        fprintf(ctx.iface, "# include <objc/message.h>\n");

        fprintf(ctx.impl, "#include \"%s.iface.h\"\n", target_name);
        fprintf(ctx.init, "#include \"%s.iface.h\"\n", target_name);

        c_transpiler_visitor.top_level(root, &ctx);
        c_transpiler_ctx_dump(&ctx);

        fprintf(ctx.iface, "#endif\n");


        _fd_copy(ctx.iface, "lab/a.iface.h");
        _fd_copy(ctx.impl, "lab/a.impl.c");
        _fd_copy(ctx.init, "lab/a.init.c");

     } else {
        fprintf(stderr, "Unknown visitor: %s\n", visitor);
        exit(1);
    }
}

int main(int ac, char **av) {
    const char *filename, *visitor;
    if (parse_args(ac, av, &filename, &visitor)) return 1;
    char *input = read_file(filename);
    if (!input) {
        fprintf(stderr, "Could not open %s\n", filename);
        return 1;
    }
    struct parser_ctx ctx = { .input = input, .pos = 0, .length = strlen(input) };
    top_level *root = parse_top_level(&ctx);
    if (!root) {
        fprintf(stderr, "Failed to parse input.\n");
        free(input);
        return 1;
    }
    run_visitor(visitor, root);
    free(input);
    return 0;
}