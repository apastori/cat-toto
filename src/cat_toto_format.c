/*
 * cat_toto_format.c — byte-level flag transformation for the formatted I/O path.
 *
 * Responsibility: expand input bytes per GNU cat formatting flags (-n/-b/-E/-T/-s/-v).
 * Syscalls: none (pure in-memory transform).
 * Heap: none.
 * I/O strategy: read chunk in cat_toto_io.c, transform here, write chunk there.
 * C standard: ISO C11.
 */

#include "cat_toto_format.h"

#include <stdio.h>

static size_t emit_line_number(char *out, size_t out_cap, size_t pos, long num)
{
    int num_bytes_written;

    if (pos >= out_cap) {
        return pos;
    }

    num_bytes_written = snprintf(out + pos, out_cap - pos, "%6ld\t", num);
    if (num_bytes_written < 0) {
        return pos;
    }

    return pos + (size_t)num_bytes_written;
}

static size_t emit_char(char *out, size_t out_cap, size_t pos, unsigned char c)
{
    if (pos >= out_cap) {
        return pos;
    }

    out[pos] = (char)c;
    return pos + 1;
}

static size_t emit_two(char *out, size_t out_cap, size_t pos, char a, char b)
{
    pos = emit_char(out, out_cap, pos, (unsigned char)a);
    pos = emit_char(out, out_cap, pos, (unsigned char)b);
    return pos;
}

static size_t emit_three(char *out, size_t out_cap, size_t pos, char a, char b, char c)
{
    pos = emit_char(out, out_cap, pos, (unsigned char)a);
    pos = emit_char(out, out_cap, pos, (unsigned char)b);
    pos = emit_char(out, out_cap, pos, (unsigned char)c);
    return pos;
}

static size_t emit_four(
    char *out,
    size_t out_cap,
    size_t pos,
    char a,
    char b,
    char c,
    char d)
{
    pos = emit_char(out, out_cap, pos, (unsigned char)a);
    pos = emit_char(out, out_cap, pos, (unsigned char)b);
    pos = emit_char(out, out_cap, pos, (unsigned char)c);
    pos = emit_char(out, out_cap, pos, (unsigned char)d);
    return pos;
}

/*
 * Emit -v notation for a non-printing byte.
 * Preconditions: c is not printable ASCII; c is not tab or newline.
 * Postcondition: returns updated output position.
 */
static size_t emit_show_nonprinting(
    char *out,
    size_t out_cap,
    size_t pos,
    unsigned char c)
{
    if (c == 0x7FU) {
        return emit_two(out, out_cap, pos, '^', '?');
    }

    if (c <= 0x1FU) {
        return emit_two(out, out_cap, pos, '^', (char)(c + 0x40U));
    }

    if (c >= 0x80U && c <= 0x9FU) {
        return emit_four(
            out,
            out_cap,
            pos,
            'M',
            '-',
            '^',
            (char)((c - 0x80U) + 0x40U));
    }

    if (c == 0xFFU) {
        return emit_four(out, out_cap, pos, 'M', '-', '^', '?');
    }

    if (c >= 0xA0U && c <= 0xFEU) {
        return emit_three(out, out_cap, pos, 'M', '-', (char)(c - 0x80U));
    }

    return emit_char(out, out_cap, pos, c);
}

void cat_toto_format_state_init(format_state_t *state)
{
    state->line_num = 1;
    state->at_bol = true;
    state->blank_run = 0;
}

/*
 * Transform one input chunk into formatted output.
 *
 * Preconditions: in_len > 0, out_cap >= in_len * 32, opts != NULL, state != NULL.
 * Postcondition: state updated; return value is bytes written to out_buf.
 * Return: number of bytes placed in out_buf (always <= out_cap).
 */
size_t cat_toto_format_chunk(
    const char *in_buf,
    size_t in_len,
    char *out_buf,
    size_t out_cap,
    const cat_opts_t *opts,
    format_state_t *state)
{
    const bool number_nonblank = opts->number_nonblank;
    const bool number_all = opts->number_all;
    const bool show_nonprinting = opts->show_nonprinting;
    const bool show_tabs = opts->show_tabs;
    const bool show_ends = opts->show_ends;
    const bool squeeze_blank = opts->squeeze_blank;
    size_t pos = 0;
    size_t i;

    for (i = 0; i < in_len; i++) {
        unsigned char c = (unsigned char)in_buf[i];
        bool discarded = false;

        /* Counting and discarding blank lines */
        if (squeeze_blank && state->at_bol && c == '\n') {
            state->blank_run++;
            if (state->blank_run > 1) {
                discarded = true;
            }
        }

        if (discarded) {
            continue;
        }

        if (state->at_bol) {
            if (number_nonblank) {
                if (c != '\n') {
                    pos = emit_line_number(
                        out_buf, out_cap, pos, state->line_num);
                    state->line_num++;
                }
            } 
            if (number_all) {
                pos = emit_line_number(
                    out_buf, out_cap, pos, state->line_num);
                state->line_num++;
            }
        }

        /* if show_tabs is true and c is a tab, emit '^I' */
        int should_emit_tab = show_tabs && c == '\t';
        if (should_emit_tab) {
            pos = emit_two(out_buf, out_cap, pos, '^', 'I');
        } 
        
        /* if show_nonprinting is true and c is not a newline or tab, emit the nonprinting character */
        int should_emit_nonprinting = show_nonprinting && c != '\n' && c != '\t';
        if (!should_emit_tab && should_emit_nonprinting) {
            /*  0x20U is equivalent to 32 in decimal, 0x7FU is equivalent to 127 in decimal, 0x80U is equivalent to 128 in decimal */
            /* less than 32 matches ASCII Control Codes, 127 matches DEL control code, greater or equal than 128 matches extended ASCII */
            /* all this characters are non-printing */
            if (c < 0x20U || c == 0x7FU || c >= 0x80U) {
                pos = emit_show_nonprinting(out_buf, out_cap, pos, c);
            } 
            /* if c is printable ASCII byte/character, emit it */
            if (c >= 0x20U && c != 0x7FU && c < 0x80U) {
                pos = emit_char(out_buf, out_cap, pos, c);
            }
        } 
        if (!should_emit_tab && !should_emit_nonprinting) {
            if (show_ends && c == '\n') {
                pos = emit_char(out_buf, out_cap, pos, '$');
            }
            pos = emit_char(out_buf, out_cap, pos, c);
        }

        if (c != '\n') {
            state->blank_run = 0;
            state->at_bol = false;
        }

        if (c == '\n') {
            state->at_bol = true;
        }
    }

    return pos;
}
