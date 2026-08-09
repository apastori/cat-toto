/*
 * test_fmt_number_nonblank.c — unit tests for -b / number-nonblank formatting.
 *
 * Responsibility: verify blank lines are not numbered and -b overrides -n.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_number_nonblank.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_number_nonblank(void)
{
    const char input[] = "a\n\nb\n";
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    opts.number_nonblank = true;

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        input,
        sizeof(input) - 1U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 19U);
    assert(memcmp(out, "     1\ta\n\n     2\tb\n", n) == 0);
    assert(state.line_num == 3);

    opts.number_all = true;
    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        input,
        sizeof(input) - 1U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 19U);
    assert(memcmp(out, "     1\ta\n\n     2\tb\n", n) == 0);

    puts("PASS: number_nonblank");
}
