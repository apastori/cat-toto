# cat-toto

GNU `cat`-compatible utility that reads files sequentially and writes their
contents to standard output. With no file operands, or when `-` is given as an
operand, input is read from standard input. Multiple files are concatenated in
order; line numbers (when requested) continue across files.

## Build

```sh
make              # release binary: cat-toto
make debug        # cat-toto-debug with sanitizers
make test         # build and run format-engine unit tests
make clean        # remove build artefacts
make install      # install to /usr/local/bin (Linux/macOS)
```

**Linux:**

```sh
make clean && make
```

**Windows (MSYS2 UCRT64):**

Open the MSYS2 UCRT64 terminal, then:

```sh
pacman -S make mingw-w64-ucrt-x86_64-gcc   # once
cd /path/to/cat-toto
make clean && make
```

If using Git Bash:

```sh
PATH="/c/msys64/ucrt64/bin:$PATH" make
```

The resulting native Windows executable runs in UCRT64, Git Bash, cmd, and
PowerShell.

## Usage

```sh
cat-toto [OPTION]... [FILE]...
```

With no `FILE`, or when `FILE` is `-`, read standard input. Multiple files are
written in the order given; `-` may appear between operands to read stdin at
that point.

| Flag | Long form | Meaning |
|------|-----------|---------|
| `-n` | `--number` | Number all output lines |
| `-b` | `--number-nonblank` | Number non-empty lines only |
| `-E` | `--show-ends` | Show `$` at end of each line |
| `-T` | `--show-tabs` | Show TAB as `^I` |
| `-s` | `--squeeze-blank` | Suppress repeated adjacent blank lines |
| `-v` | `--show-nonprinting` | Show non-printing chars using `^` and `M-` notation |
| `-A` | `--show-all` | Equivalent to `-vET` |
| `-e` | | Equivalent to `-vE` |
| `-t` | | Equivalent to `-vT` |
| `-u` | | Ignored (POSIX compatibility) |
| | `--help` | Print help and exit 0 |
| | `--version` | Print version and exit 0 |

`-b` overrides `-n`. `-A` is equivalent to `-vET`; `-e` to `-vE`; `-t` to
`-vT`. Use `--` to stop flag parsing; all following tokens are file operands.
Line numbers are continuous across multiple file operands.

## Exit codes

| Code | Meaning |
|------|---------|
| `0` | Success; `--help`; `--version`; broken pipe when writing to a closed pipe |
| `1` | File open/read error (message on stderr, processing continues with remaining operands); write error (fatal); invalid flag (fatal); signal setup failure |
| `130` | Interrupted by Ctrl+C (`SIGINT`) |

On Linux, `SIGPIPE` is ignored so a closed pipe is reported as `EPIPE` from
`write()` rather than killing the process. stderr messages use the form
`cat-toto: <context>: <reason>`.

## Layout

```
cat-toto/
├── LICENSE.txt
├── c_version.txt
├── Makefile
├── README.md
├── include/
│   ├── cat_toto.h
│   ├── cat_toto_emit.h
│   ├── cat_toto_format.h
│   └── cat_toto_cli.h
├── src/
│   ├── main.c
│   ├── cat_toto_emit.c
│   ├── cat_toto_io.c
│   ├── cat_toto_format.c
│   └── cat_toto_cli.c
└── tests/
    ├── test_runner.c
    └── test_fmt_*.c / .h  →  tests/test_core
```
