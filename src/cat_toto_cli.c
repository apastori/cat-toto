/*
 * cat_toto_cli.c — CLI parsing, help/version text, and signal setup.
 *
 * Responsibility: parse flags, print usage, install SIGPIPE/SIGINT handlers.
 * Syscalls: write(2) on STDOUT_FILENO; sigaction/signal on POSIX/Windows.
 * Heap: none.
 * I/O strategy: cold-path stdout writes for help/version only.
 * C standard: ISO C11.
 */

#include "cat_toto_cli.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cat_toto_emit.h"

static volatile sig_atomic_t cat_toto_stop_requested = 0;

static void handle_sigint(int signo)
{
    (void)signo;
    cat_toto_stop_requested = 1;
}

static void apply_short_flag(char flag, cat_opts_t *opts)
{
    switch (flag) {
    case 'n':
        opts->number_all = true;
        break;
    case 'b':
        opts->number_nonblank = true;
        break;
    case 'E':
        opts->show_ends = true;
        break;
    case 'T':
        opts->show_tabs = true;
        break;
    case 's':
        opts->squeeze_blank = true;
        break;
    case 'v':
        opts->show_nonprinting = true;
        break;
    case 'A':
        opts->show_nonprinting = true;
        opts->show_ends = true;
        opts->show_tabs = true;
        break;
    case 'e':
        opts->show_nonprinting = true;
        opts->show_ends = true;
        break;
    case 't':
        opts->show_nonprinting = true;
        opts->show_tabs = true;
        break;
    case 'u':
        break;
    default:
        cat_toto_emit_bad_flag(flag);
        exit(CAT_TOTO_EXIT_ERR);
    }
}

static void apply_long_flag(const char *arg, cat_opts_t *opts)
{
    if (strcmp(arg, "help") == 0) {
        print_help();
        exit(CAT_TOTO_EXIT_OK);
    }
    if (strcmp(arg, "version") == 0) {
        print_version();
        exit(CAT_TOTO_EXIT_OK);
    }
    if (strcmp(arg, "number") == 0) {
        opts->number_all = true;
        return;
    }
    if (strcmp(arg, "number-nonblank") == 0) {
        opts->number_nonblank = true;
        return;
    }
    if (strcmp(arg, "show-all") == 0) {
        opts->show_nonprinting = true;
        opts->show_ends = true;
        opts->show_tabs = true;
        return;
    }
    if (strcmp(arg, "show-ends") == 0) {
        opts->show_ends = true;
        return;
    }
    if (strcmp(arg, "show-tabs") == 0) {
        opts->show_tabs = true;
        return;
    }
    if (strcmp(arg, "show-nonprinting") == 0) {
        opts->show_nonprinting = true;
        return;
    }
    if (strcmp(arg, "squeeze-blank") == 0) {
        opts->squeeze_blank = true;
        return;
    }

    cat_toto_emit_bad_flag('-');
    exit(CAT_TOTO_EXIT_ERR);
}

static void write_stdout_literal(const char *text)
{
    size_t len = strlen(text);

    if (len > 0U) {
        (void)write(STDOUT_FILENO, text, len);
    }
}

void print_version(void)
{
    char buf[64];
    int n;

    n = snprintf(buf, sizeof(buf), "cat-toto %s\n", CAT_TOTO_VERSION_STRING);
    if (n > 0) {
        (void)write(STDOUT_FILENO, buf, (size_t)n);
    }
}

void print_help(void)
{
    write_stdout_literal(
        "Usage: cat-toto [OPTION]... [FILE]...\n"
        "Concatenate FILE(s) to standard output.\n"
        "\n"
        "  -A, --show-all           equivalent to -vET\n"
        "  -b, --number-nonblank    number nonempty output lines\n"
        "  -e                       equivalent to -vE\n"
        "  -E, --show-ends          display $ at end of each line\n"
        "  -n, --number             number all output lines\n"
        "  -s, --squeeze-blank      suppress repeated empty output lines\n"
        "  -t                       equivalent to -vT\n"
        "  -T, --show-tabs          display TAB characters as ^I\n"
        "  -u                       (ignored)\n"
        "  -v, --show-nonprinting   use ^ and M- notation\n"
        "      --help               display this help and exit\n"
        "      --version            output version information and exit\n"
        "\n"
        "With no FILE, or when FILE is -, read standard input.\n");
}

/*
 * Parse command-line flags and locate the first file operand.
 *
 * Preconditions: argc >= 1, argv != NULL, opts != NULL,
 *                first_operand_index != NULL.
 * Postcondition: opts fields set; *first_operand_index is index of first
 *                operand or argc if none remain.
 * Error behavior: exits on --help, --version, or invalid flag.
 */
void parse_flags(int argc, char **argv, cat_opts_t *opts, int *first_operand_index)
{
    int i;

    opts->number_all = false;
    opts->number_nonblank = false;
    opts->show_ends = false;
    opts->show_tabs = false;
    opts->squeeze_blank = false;
    opts->show_nonprinting = false;

    for (i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--") == 0) {
            *first_operand_index = i + 1;
            return;
        }

        if (arg[0] == '-' && arg[1] != '\0') {
            if (arg[1] == '-' && arg[2] == '\0') {
                *first_operand_index = i + 1;
                return;
            }

            if (arg[1] == '-' && arg[2] != '\0') {
                apply_long_flag(arg + 2, opts);
                continue;
            }

            for (int j = 1; arg[j] != '\0'; j++) {
                apply_short_flag(arg[j], opts);
            }
            continue;
        }

        *first_operand_index = i;
        return;
    }

    *first_operand_index = argc;
}

int install_sigpipe_ignore(void)
{
#if defined(_WIN32)
    return 0;
#else
    struct sigaction sa;

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGPIPE, &sa, NULL) != 0) {
        return -1;
    }

    return 0;
#endif
}

int install_sigint_handler(void)
{
#if defined(_WIN32)
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        return -1;
    }
#else
    struct sigaction sa;

    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) != 0) {
        return -1;
    }
#endif

    return 0;
}

void cat_toto_exit_if_stop_requested(void)
{
    if (cat_toto_stop_requested) {
        _exit(CAT_TOTO_EXIT_SIGINT);
    }
}
