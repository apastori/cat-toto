/*
 * test_runner.c — runs all cat_toto_format unit test suites.
 *
 * Responsibility: invoke each test_fmt_* function in fixed order.
 * Syscalls: none (test output via puts only).
 * Heap: none.
 * I/O strategy: stdout for PASS lines only.
 * C standard: ISO C11.
 */

#include <stdio.h>

#include "test_fmt_number_all.h"
#include "test_fmt_number_nonblank.h"
#include "test_fmt_passthrough.h"
#include "test_fmt_show_all.h"
#include "test_fmt_show_ends.h"
#include "test_fmt_show_nonprinting.h"
#include "test_fmt_show_tabs.h"
#include "test_fmt_squeeze_blank.h"

int main(void)
{
    test_fmt_passthrough();
    test_fmt_show_ends();
    test_fmt_show_tabs();
    test_fmt_number_all();
    test_fmt_number_nonblank();
    test_fmt_squeeze_blank();
    test_fmt_show_nonprinting();
    test_fmt_show_all();

    puts("All tests passed.");
    return 0;
}
