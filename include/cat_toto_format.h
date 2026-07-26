#ifndef CAT_TOTO_FORMAT_H
#define CAT_TOTO_FORMAT_H

#include <stddef.h>

#include "cat_toto.h"

void cat_toto_format_state_init(format_state_t *state);

size_t cat_toto_format_chunk(
    const char *in_buf,
    size_t in_len,
    char *out_buf,
    size_t out_cap,
    const cat_opts_t *opts,
    format_state_t *state);

#endif /* CAT_TOTO_FORMAT_H */
