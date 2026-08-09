/*
 * test_fmt_show_tabs.c — unit tests for -T / show-tabs formatting.
 *
 * Responsibility: verify TAB expands to ^I.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_show_tabs.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_show_tabs(void)
{
    const char input[] = "a\tb\tc";
    const char expected[] = "a^Ib^Ic";
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    opts.show_tabs = true;

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
        "x\ny",
        3U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 3U);
    assert(memcmp(out, "x\ny", 3U) == 0);

    puts("PASS: show_tabs");
}
