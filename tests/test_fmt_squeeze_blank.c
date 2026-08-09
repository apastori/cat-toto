/*
 * test_fmt_squeeze_blank.c — unit tests for -s / squeeze-blank formatting.
 *
 * Responsibility: verify consecutive blank lines collapse to one.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_squeeze_blank.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_squeeze_blank(void)
{
    const char input[] = "a\n\n\nb\n";
    const char expected[] = "a\n\nb\n";
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    opts.squeeze_blank = true;

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        input,
        sizeof(input) - 1U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == sizeof(expected) - 1U);
    assert(memcmp(out, expected, n) == 0);

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        "\n\n\n\n",
        4U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 1U);
    assert(out[0] == '\n');

    puts("PASS: squeeze_blank");
}
