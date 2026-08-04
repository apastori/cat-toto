/*
 * cat_toto_io.c — file I/O orchestration and raw read/write loops.
 *
 * Responsibility: select pass-through vs formatted path, process operands.
 * Syscalls: open, read, write, close on file descriptors.
 * Heap: none (stack buffers only).
 * I/O strategy: 64 KiB pass-through or 2 KiB read + format + write.
 * C standard: ISO C11.
 */

#include "cat_toto.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cat_toto_cli.h"
#include "cat_toto_emit.h"
#include "cat_toto_format.h"

#if defined(_WIN32)
#include <io.h>
#endif

static bool needs_format(const cat_opts_t *opts)
{
    return opts->number_all
        || opts->number_nonblank
        || opts->show_ends
        || opts->show_tabs
        || opts->squeeze_blank
        || opts->show_nonprinting;
}

#if defined(_WIN32)
static void maybe_normalize_broken_pipe_errno(int fd)
{
    if (errno == EINVAL && _isatty(fd) == 0) {
        errno = EPIPE;
    }
}
#else
static void maybe_normalize_broken_pipe_errno(int fd)
{
    (void)fd;
}
#endif

/*
 * Write all bytes to fd, retrying on partial writes and EINTR.
 *
 * Preconditions: fd >= 0, buf != NULL or len == 0.
 * Postcondition: all bytes written, or process exits (never returns on error).
 * Error behavior: EPIPE -> fflush(stdout) + exit(0); other errors -> exit(1).
 */
static void write_all(int fd, const void *buf, size_t len)
{
    const unsigned char *p = (const unsigned char *)buf;
    size_t remaining = len;

    while (remaining > 0U) {
        ssize_t num_bytes_written = write(fd, p, remaining);

        if (num_bytes_written < 0) {
            if (errno == EINTR) {
                continue;
            }

            maybe_normalize_broken_pipe_errno(fd);

            if (errno == EPIPE) {
                (void)fflush(stdout);
                exit(CAT_TOTO_EXIT_OK);
            }

            cat_toto_emit_error("write");
            exit(CAT_TOTO_EXIT_ERR);
        }

        if (num_bytes_written == 0) {
            cat_toto_emit_error("write");
            exit(CAT_TOTO_EXIT_ERR);
        }

        p += (size_t)num_bytes_written;
        remaining -= (size_t)num_bytes_written;
    }
}

static void process_file_passthrough(
    int fd,
    const char *filename,
    int *exit_status)
{
    char in_buf[CAT_TOTO_BUF_SIZE];

    /* Infinite loop the same as while(true) { ... } */
    for (;;) {
        ssize_t num_bytes_read = read(fd, in_buf, sizeof(in_buf));

        if (num_bytes_read == 0) {
            break;
        }

        if (num_bytes_read < 0) {
            if (errno == EINTR) {
                continue;
            }
            cat_toto_emit_error(filename);
            *exit_status = CAT_TOTO_EXIT_ERR;
            return;
        }

        cat_toto_exit_if_stop_requested();
        write_all(STDOUT_FILENO, in_buf, (size_t)num_bytes_read);
    }
}

static void process_file_formatted(
    int fd,
    const char *filename,
    const cat_opts_t *opts,
    format_state_t *state,
    int *exit_status)
{
    char in_buf[CAT_TOTO_FMT_IN_SIZE];
    char out_buf[CAT_TOTO_FMT_OUT_SIZE];

    /* Infinite loop the same as while(true) { ... } */
    for (;;) {
        ssize_t num_bytes_read = read(fd, in_buf, sizeof(in_buf));

        if (num_bytes_read == 0) {
            break;
        }

        if (num_bytes_read < 0) {
            if (errno == EINTR) {
                continue;
            }
            cat_toto_emit_error(filename);
            *exit_status = CAT_TOTO_EXIT_ERR;
            return;
        }

        cat_toto_exit_if_stop_requested();

        {
            size_t modified_bytes_filled = cat_toto_format_chunk(
                in_buf,
                (size_t)num_bytes_read,
                out_buf,
                sizeof(out_buf),
                opts,
                state);

            if (modified_bytes_filled > 0U) {
                write_all(STDOUT_FILENO, out_buf, modified_bytes_filled);
            }
        }
    }
}

/*
 * Read one open file descriptor and write to stdout.
 *
 * Preconditions: fd >= 0, filename != NULL, opts != NULL, state != NULL,
 *                exit_status != NULL.
 * Postcondition: file consumed or read error recorded in *exit_status.
 */
static void process_file(
    int fd,
    const char *filename,
    const cat_opts_t *opts,
    format_state_t *state,
    int *exit_status)
{
    if (needs_format(opts)) {
        process_file_formatted(fd, filename, opts, state, exit_status);
        return;
    } 
    
    if (!needs_format(opts)) {
        process_file_passthrough(fd, filename, exit_status);
        return;
    }
}

static bool is_stdin_operand(const char *operand)
{
    return operand[0] == '-' && operand[1] == '\0';
}

static bool use_stdin_only_path(int operand_count, char **operands)
{
    if (operand_count == 0) {
        return true;
    }

    if (operand_count == 1 && is_stdin_operand(operands[0])) {
        return true;
    }

    return false;
}

/*
 * Process all file operands and return final exit status.
 *
 * Preconditions: opts != NULL; operands may be NULL when operand_count is 0.
 * Postcondition: returns 0 or 1; write errors exit immediately from callee.
 */
int cat_toto_run(const cat_opts_t *opts, int operand_count, char **operands)
{
    int exit_status = CAT_TOTO_EXIT_OK;
    format_state_t state;
    int i;

    cat_toto_format_state_init(&state);

    if (use_stdin_only_path(operand_count, operands)) {
        process_file(STDIN_FILENO, "(stdin)", opts, &state, &exit_status);
        return exit_status;
    }

    for (i = 0; i < operand_count; i++) {
        const char *operand = operands[i];

        if (is_stdin_operand(operand)) {
            process_file(STDIN_FILENO, "(stdin)", opts, &state, &exit_status);
            continue;
        }

        {
            int fd = open(operand, O_RDONLY);

            if (fd < 0) {
                cat_toto_emit_error(operand);
                exit_status = CAT_TOTO_EXIT_ERR;
                continue;
            }

            process_file(fd, operand, opts, &state, &exit_status);
            (void)close(fd);
        }
    }

    return exit_status;
}
