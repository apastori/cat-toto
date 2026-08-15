# cat-toto — POSIX-oriented Makefile (gcc or clang on Unix, MSYS2, etc.)
#
# Compiler flag rationale (release CFLAGS):
#   -std=c11          ISO C11 baseline requested by the project spec.
#   -O2               Strong optimisation without -O3's aggressive trade-offs.
#   -Wall -Wextra     Enable most warnings.
#   -Wpedantic        Reject common extensions and dubious constructs.
#   -Werror           Treat warnings as build-breaking (zero-warning policy).
#   -D_POSIX_C_SOURCE=200809L  Expose POSIX.1-2008 (open, read, write, etc.).
#
# Artefacts live under build/ (objects + binaries). Tests under build/tests/.

CC := $(shell command -v gcc >/dev/null 2>&1 && echo gcc || echo clang)

CFLAGS_COMMON := -std=c11 -Wall -Wextra -Wpedantic -Werror \
	-D_POSIX_C_SOURCE=200809L -Iinclude

CFLAGS := $(CFLAGS_COMMON) -O2

# Sanitizers need matching compile+link flags.
CFLAGS_DEBUG := $(CFLAGS_COMMON) -g -O1 -fsanitize=address,undefined \
	-fno-omit-frame-pointer

BUILD_DIR := build
TEST_BUILD_DIR := $(BUILD_DIR)/tests

MAIN_SRCS := src/main.c \
	src/cat_toto_emit.c \
	src/cat_toto_io.c \
	src/cat_toto_format.c \
	src/cat_toto_cli.c

HDRS := $(wildcard include/*.h)

TEST_SRCS := tests/test_runner.c \
	tests/test_fmt_passthrough.c \
	tests/test_fmt_show_ends.c \
	tests/test_fmt_show_tabs.c \
	tests/test_fmt_number_all.c \
	tests/test_fmt_number_nonblank.c \
	tests/test_fmt_squeeze_blank.c \
	tests/test_fmt_show_nonprinting.c \
	tests/test_fmt_show_all.c

TEST_HDRS := $(wildcard tests/*.h)

MAIN_OBJS := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(MAIN_SRCS))
TEST_OBJS := $(patsubst tests/%.c,$(TEST_BUILD_DIR)/%.o,$(TEST_SRCS))

CAT_TOTO := $(BUILD_DIR)/cat-toto
CAT_TOTO_DEBUG := $(BUILD_DIR)/cat-toto-debug
TEST_CORE := $(TEST_BUILD_DIR)/test_core

.PHONY: all debug test clean install

all: $(CAT_TOTO)

$(CAT_TOTO): $(MAIN_OBJS) $(HDRS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJS)

debug: $(CAT_TOTO_DEBUG)

# Debug/sanitizer binary: compile+link from sources with CFLAGS_DEBUG
# (must not reuse release .o files built with -O2).
$(CAT_TOTO_DEBUG): $(MAIN_SRCS) $(HDRS) | $(BUILD_DIR)
	$(CC) $(CFLAGS_DEBUG) -o $@ $(MAIN_SRCS)

$(TEST_CORE): $(TEST_OBJS) $(BUILD_DIR)/cat_toto_format.o $(HDRS) $(TEST_HDRS) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(BUILD_DIR)/cat_toto_format.o

test: $(TEST_CORE)
	./$(TEST_CORE)

$(BUILD_DIR)/%.o: src/%.c $(HDRS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(TEST_BUILD_DIR)/%.o: tests/%.c $(HDRS) $(TEST_HDRS) | $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR) $(TEST_BUILD_DIR):
	mkdir -p $@

clean:
	rm -f $(MAIN_OBJS) $(TEST_OBJS) $(CAT_TOTO) $(CAT_TOTO_DEBUG) $(TEST_CORE)
	rm -f $(CAT_TOTO).exe $(CAT_TOTO_DEBUG).exe $(TEST_CORE).exe

install: $(CAT_TOTO)
	install -m 755 $(CAT_TOTO) /usr/local/bin
