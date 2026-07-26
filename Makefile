# cat-toto — GNU Make build (C11, libc only)
# Release uses -O2; debug adds ASan/UBSan. All warnings are errors (-Werror).

CC := $(shell command -v gcc >/dev/null 2>&1 && echo gcc || echo clang)

CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror \
          -D_POSIX_C_SOURCE=200809L -Iinclude -O2

DEBUG_CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror \
                -D_POSIX_C_SOURCE=200809L -Iinclude -g -O1 \
                -fsanitize=address,undefined -fno-omit-frame-pointer

SRCS := src/main.c \
        src/cat_toto_emit.c \
        src/cat_toto_io.c \
        src/cat_toto_format.c \
        src/cat_toto_cli.c

HDRS := include/cat_toto.h \
        include/cat_toto_emit.h \
        include/cat_toto_format.h \
        include/cat_toto_cli.h

TEST_SRCS := tests/test_runner.c \
             tests/test_fmt_passthrough.c \
             tests/test_fmt_show_ends.c \
             tests/test_fmt_show_tabs.c \
             tests/test_fmt_number_all.c \
             tests/test_fmt_number_nonblank.c \
             tests/test_fmt_squeeze_blank.c \
             tests/test_fmt_show_nonprinting.c \
             tests/test_fmt_show_all.c

.PHONY: all debug test clean install

all: cat-toto

cat-toto: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

debug: cat-toto-debug

cat-toto-debug: $(SRCS) $(HDRS)
	$(CC) $(DEBUG_CFLAGS) -o $@ $(SRCS)

tests/test_core: $(TEST_SRCS) src/cat_toto_format.c $(HDRS) $(wildcard tests/*.h)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRCS) src/cat_toto_format.c

test: tests/test_core
	./tests/test_core

clean:
	rm -f cat-toto cat-toto-debug cat-toto.exe cat-toto-debug.exe \
	      tests/test_core tests/test_core.exe

install: cat-toto
	install -m 755 cat-toto /usr/local/bin
