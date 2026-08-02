/*
 * cat_toto_emit.c — stderr diagnostics for cat-toto.
 *
 * Responsibility: format and emit error messages to stderr.
 * Syscalls: write(2) on STDERR_FILENO.
 * Heap: none (stack buffer for message assembly).
 * I/O strategy: cold-path write only; not used in hot read/write loops.
 * C standard: ISO C11.
 */

#include "cat_toto_emit.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void cat_toto_emit_error(const char *context)
{
    char buf[512];
    int num_bytes_written;

    num_bytes_written = snprintf(
        buf,
        sizeof(buf),
        "cat-toto: %s: %s\n",
        context,
        strerror(errno));
    if (num_bytes_written > 0) {
        (void)write(STDERR_FILENO, buf, (size_t)num_bytes_written);
    }
}

void cat_toto_emit_bad_flag(const char *flag)
{
    char buf[128];
    int n;

    n = snprintf(
        buf,
        sizeof(buf),
        "cat-toto: invalid option -- '%s'\n",
        flag);
    if (n > 0) {
        (void)write(STDERR_FILENO, buf, (size_t)n);
    }
}
