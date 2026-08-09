/*
 * test_fmt_passthrough.c — unit tests for unformatted passthrough formatting.
 *
 * Responsibility: verify cat_toto_format_chunk with no flags set.
 * Syscalls: none.
 * Heap: none.
 * I/O strategy: in-memory buffers only.
 * C standard: ISO C11.
 */

#include "test_fmt_passthrough.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "cat_toto_format.h"

void test_fmt_passthrough(void)
{
    const char input[] = "hello\nworld";
    char out[CAT_TOTO_FMT_OUT_SIZE];
    cat_opts_t opts = {0};
    format_state_t state;
    size_t n;

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        input,
        sizeof(input) - 1U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == sizeof(input) - 1U);
    assert(memcmp(out, input, n) == 0);
    assert(state.at_bol == false);

    cat_toto_format_state_init(&state);
    n = cat_toto_format_chunk(
        "line\n",
        5U,
        out,
        sizeof(out),
        &opts,
        &state);

    assert(n == 5U);
    assert(memcmp(out, "line\n", 5U) == 0);
    assert(state.at_bol == true);

    puts("PASS: passthrough");
}
