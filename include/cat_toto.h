#ifndef CAT_TOTO_H
#define CAT_TOTO_H

#include <stdbool.h>

#define CAT_TOTO_BUF_SIZE       65536
#define CAT_TOTO_FMT_IN_SIZE    2048
#define CAT_TOTO_FMT_OUT_SIZE   65536

#define CAT_TOTO_VERSION_STRING "1.0.0"

typedef enum {
    CAT_TOTO_EXIT_OK = 0,
    CAT_TOTO_EXIT_ERR = 1,
    CAT_TOTO_EXIT_SIGINT = 130
} cat_toto_exit_code_t;

typedef struct {
    bool number_all;
    bool number_nonblank;
    bool show_ends;
    bool show_tabs;
    bool squeeze_blank;
    bool show_nonprinting;
} cat_opts_t;

typedef struct {
    long line_num;
    bool at_bol;
    int blank_run;
} format_state_t;

int cat_toto_run(const cat_opts_t *opts, int operand_count, char **operands);

#endif /* CAT_TOTO_H */
