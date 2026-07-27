/*
 * main.c — thin entry point for cat-toto.
 *
 * Responsibility: install signals, parse CLI, dispatch to cat_toto_run().
 * Syscalls: none directly (delegated to other modules).
 * Heap: none.
 * I/O strategy: none here.
 * C standard: ISO C11.
 */

#include <stdlib.h>

#include "cat_toto.h"
#include "cat_toto_cli.h"
#include "cat_toto_emit.h"

int main(int argc, char **argv)
{
    cat_opts_t opts;
    int first_operand;

    if (install_sigpipe_ignore() != 0) {
        cat_toto_emit_error("sigpipe");
        return CAT_TOTO_EXIT_ERR;
    }

    if (install_sigint_handler() != 0) {
        cat_toto_emit_error("sigint");
        return CAT_TOTO_EXIT_ERR;
    }

    parse_flags(argc, argv, &opts, &first_operand);

    return cat_toto_run(
        &opts,
        argc - first_operand,
        argv + first_operand);
}
