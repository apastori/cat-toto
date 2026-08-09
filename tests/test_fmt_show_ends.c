/*
 * test_fmt_show_ends.c — unit tests for -E / show-ends formatting.
 *
 * Responsibility: verify $ is inserted before each newline.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_show_ends.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_show_ends(void)
{
    const char input[] = "hello\nworld\n";
    const char expected[] = "hello$\nworld$\n";
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    opts.show_ends = true;

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
        "hello",
        5U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 5U);
    assert(memcmp(out, "hello", 5U) == 0);

    puts("PASS: show_ends");
}
