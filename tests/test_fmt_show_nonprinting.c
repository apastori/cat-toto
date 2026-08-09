/*
 * test_fmt_show_nonprinting.c — unit tests for -v / show-nonprinting formatting.
 *
 * Responsibility: verify caret and M- notation for non-printing bytes.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_show_nonprinting.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_show_nonprinting(void)
{
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    opts.show_nonprinting = true;

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk("\x01", 1U, out, sizeof(out), &opts, &state);
    assert(n == 2U);
    assert(memcmp(out, "^A", 2U) == 0);

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk("\x7f", 1U, out, sizeof(out), &opts, &state);
    assert(n == 2U);
    assert(memcmp(out, "^?", 2U) == 0);

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk("\x81", 1U, out, sizeof(out), &opts, &state);
    assert(n == 4U);
    assert(memcmp(out, "M-^A", 4U) == 0);

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk("\xfe", 1U, out, sizeof(out), &opts, &state);
    assert(n == 3U);
    assert(memcmp(out, "M-~", 3U) == 0);

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk("a\tb\n", 4U, out, sizeof(out), &opts, &state);
    assert(n == 4U);
    assert(memcmp(out, "a\tb\n", 4U) == 0);

    puts("PASS: show_nonprinting");
}
