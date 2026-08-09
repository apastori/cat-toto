/*
 * test_fmt_number_all.c — unit tests for -n / number-all formatting.
 *
 * Responsibility: verify every line receives a number prefix.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_number_all.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_number_all(void)
{
    const char input[] = "a\nb\nc\n";
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    opts.number_all = true;

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        input,
        sizeof(input) - 1U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 27U);
    assert(memcmp(out, "     1\ta\n     2\tb\n     3\tc\n", n) == 0);
    assert(state.line_num == 4);

    puts("PASS: number_all");
}
