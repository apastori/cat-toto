#ifndef CAT_TOTO_CLI_H
#define CAT_TOTO_CLI_H

#include "cat_toto.h"

void parse_flags(int argc, char **argv, cat_opts_t *opts, int *first_operand_index);
void print_help(void);
void print_version(void);
int install_sigpipe_ignore(void);
int install_sigint_handler(void);
void cat_toto_exit_if_stop_requested(void);

#endif /* CAT_TOTO_CLI_H */
