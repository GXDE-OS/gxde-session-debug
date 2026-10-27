# GXDE Session Debug

## Project Overview

gxde-session-debug is a small C utility for debugging GXDE desktop-environment sessions.

GXDE ships two session types: Wayland and X11. Bugs are often session-specific — some are reported only under the Wayland session, others only under X11 — which makes them hard to reproduce and debug inside a real login session. This tool addresses that by running a nested Wayland compositor or X11 server and launching a complete GXDE session in isolated processes. The result is a disposable, restartable sandbox for reproducing and debugging session-specific bugs without disturbing the desktop the developer is currently using.

## Repository State

The repository is in an early, documentation-plus-build-scaffold stage:

- `README.md` — project background, motivation, and coding conventions (written in Chinese).
- `Makefile` — strictly POSIX-style build rules; compiles every file listed in `SRCS` under `src/`, and builds the test binaries listed in `TEST_OBJS` for `check`.
- `src/utils/log/` — logging module (`log.c`, `log.h`, `ansi_escape_code_def.h`): leveled, ANSI-colored output (`[INFO]`, `[ OK ]`, `[WARNING]`, `[ERROR]`), errors going to `stderr` and everything else to `stdout`. It is already wired into the build; `-Isrc/utils/log` is on `CPPFLAGS`, so `#include "log.h"` works from anywhere.
- `src/utils/process_handler/` — process-management module (`process_handler.c`, `process_handler.h`). Currently it provides `fork_process(program, argv)`, which forks and `execvp()`s `program` in the child with the caller-supplied, NULL-terminated argument vector. Only the parent returns, with the child PID; `-1` means the fork failed (logged with `LOG_ERROR`), while a failing `execvp()` is logged and turns into an `EXIT_FAILURE` exit status the parent sees when it waits. `-Isrc/utils/process_handler` is on `CPPFLAGS`.
- `tests/` — one test program per module, named `<module>_test.c` (`process_handler_test.c` at the moment), run through `make check`.
- `LICENSE` — GNU GPL-3.0.
- `.gitignore` — patterns for C/C++ and kernel-module build artifacts (`.o`, `*.ko`, `*.mod*`, `*.dwo`, etc.) and the compiled `gxde-session-debug` binary.

`src/main.c` does not exist yet, so `make` still stops early; the modules themselves build cleanly and their tests pass. When introducing code, follow the conventions below.

## Building and Running

The build system is a strictly POSIX-compatible Makefile (`.POSIX:` mode, suffix rules — no GNU make extensions). Note: until the first source file `src/main.c` is added, `make` stops with "no rule to make target"; `make check` works without it.

- `make` — build the `gxde-session-debug` binary from sources under `src/`.
- `make check` — build and run the test programs under `tests/`.
- `make clean` — remove object files, test objects, and the binary.
- `make install` — install to `$(DESTDIR)$(PREFIX)/bin` (default `PREFIX=/usr/local`).
- `make uninstall` — remove the installed binary.

Variables (`CC`, `CFLAGS`, `CPPFLAGS`, `LDFLAGS`, `LDLIBS`, `PREFIX`, `DESTDIR`) can be overridden on the command line, e.g. `make CFLAGS='-O0 -g'`.

`CPPFLAGS` also carries `-D_POSIX_C_SOURCE=200809L`, because `-std=c11 -pedantic` would otherwise hide the POSIX declarations (`fork()`, `waitpid()`, ...) behind `__STRICT_ANSI__`. Keep that define when overriding `CPPFLAGS`; source files that use POSIX APIs repeat it at the top of the file for the same reason.

## Development Conventions

- **Language:** C.

- **UNIX/POSIX style:** mandatory — the project must fully follow UNIX/POSIX style. Prefer portable POSIX APIs (`unistd.h`, `sys/wait.h`, ...) and standard UNIX conventions over GNU or vendor-specific extensions where a POSIX equivalent exists, and keep the build strictly POSIX-compatible.

- **MEMORY SAFETY IS THE 1ST PRIORITY**

- **Style:** K&R, with one exception — a function definition puts the return type, signature, and opening brace on the same line:

  ```c
  int is_x11_up() {
      // Code here...
  }
  ```

- **Braces on `if`:** `if` statements must always use braces, even when the body is a single statement.

  Forbidden:

  ```c
  if (something)
      exec_something();
  ```

  Required:

  ```c
  if (something) {
      exec_something();
  }
  ```

- **Doxygen comments:** mandatory for code. Classes are exempt; everything else must be documented — especially structs related to Wayland.

- **Documentation language:** source comments and documentation must be written in English, including this file (GEMINI.md). The user-facing README.md is written in Chinese.

- **New line character:** you mush have an empty line for each file you write.
