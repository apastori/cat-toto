/*
 * test_fmt_show_all.c — unit tests for -A / show-all formatting.
 *
 * Responsibility: verify combined -vET transformations.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_show_all.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_show_all(void)
{
    const char input[] = "a\t\x01\n";
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    opts.show_nonprinting = true;
    opts.show_ends = true;
    opts.show_tabs = true;

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        input,
        sizeof(input) - 1U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 7U);
    assert(memcmp(out, "a^I^A$\n", 7U) == 0);

    puts("PASS: show_all");
}
