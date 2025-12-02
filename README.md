<p align="center">
  <img src="https://github.com/user-attachments/assets/00e305bc-7d20-4af1-a3e5-c794c76b60b5" width="600" alt="BlackBox Logo" />
</p>

[![macOS Build](https://github.com/abnore/BlackBox/actions/workflows/macos-build.yml/badge.svg)](https://github.com/abnore/BlackBox/actions/workflows/macos-build.yml)
[![Release](https://img.shields.io/github/release/abnore/BlackBox.svg)](https://github.com/abnore/BlackBox/releases/latest)

# BlackBox

**BlackBox** is a fast, minimal logging library for C — inspired by aircraft flight recorders: log everything.

> [!NOTE]
> Built and tested exclusively on **macOS (POSIX)**.
> Uses `pthread`, `unistd`, and Mach-O (`_NSGetExecutablePath`) APIs.

---

## Features

- Zero external dependencies and extremely easy to integrate
- Pure C (`blackbox.h` / `src/blackbox.c`)
- Color-coded terminal output (TTY / ANSI-aware)
- Runtime log-level control via `LOG_LEVELS=...`
- Optional **file logging** (`logs/<timestamp>.log` + `logs/latest.log`)
- Optional **stderr redirection** to a separate error log
- Thread-safe via `pthread_mutex`
- Custom runtime and compile-time assertions:
  - `ASSERT(expr, ...)`
  - `BUILD_ASSERT(cond, msg)`

---

# Getting Started

## 1) Clone the repository

```sh
git clone https://github.com/abnore/BlackBox.git
cd BlackBox
```

---

## 2) Build & Install (macOS)

BlackBox ships with a minimal Makefile that builds a shared library and can optionally install it system-wide.

### 2.1 — Build locally

```sh
make
```

This creates:

- `libblackbox.dylib`

To use the library locally, include the header from the repo:

```c
#include "blackbox.h"
```

Compile your program with:

```sh
clang main.c -L. -lblackbox -o main
```

Where:

- `-L.` tells the linker to search the current directory
- `-lblackbox` links against `libblackbox.dylib`

---

### 2.2 — Clean local build

To remove the locally-built shared library:

```sh
make clean
```

This removes:

- `libblackbox.dylib`

---

### 2.3 — Install system-wide

To install into `/usr/local`:

```sh
make install
```

This installs:

- `blackbox.h` -> `/usr/local/include/`
- `libblackbox.dylib` -> `/usr/local/lib/`

After installation, you can use BlackBox in **any** C project on your system:

```c
#include <blackbox.h>
```

Compile with:

```sh
clang main.c -lblackbox -o main
```

No `-L.` needed — `/usr/local/lib` is in the default macOS search path.

---

### 2.4 — Uninstall

```sh
make uninstall
```

This removes:

- `/usr/local/include/blackbox.h`
- `/usr/local/lib/libblackbox.dylib`

---

# Usage Example

```c
#include <blackbox.h>

int main(void) {
    // LOG_DEFAULT will expand into: NO_LOG, LOG_COLORS, STDERR_TO_TERMINAL;
    init_log(LOG_DEFAULT);

    // Examples written out:
    // init_log(LOG, NO_COLORS, STDERR_TO_TERMINAL); // Log to file
    // init_log(LOG, NO_COLORS, STDERR_TO_LOG);      // Redirect stderr

    FATAL("Unrecoverable error (this does NOT abort, only logs it)");
    ERROR("Recoverable error; operation failed");
    WARN("Non-fatal anomaly");
    INFO("General status / progress");
    DEBUG("Developer debug information (x=%d)", 42);
    TRACE("Verbose internal tracing");

    // Example assertion (this WILL abort):
    // ASSERT(ptr != NULL, "ptr must not be NULL (id=%d)", id);

    shutdown_log();
    return 0;
}
```

> [!NOTE]
> BlackBox uses pthread mutexes internally.
> On macOS, pthreads are provided by **libSystem**, so `-pthread` is not required.

---

# Runtime Controls

### Helper functions

```c
log_set_color_output(bool enabled);
log_enable_level(log_level lvl);
log_disable_level(log_level lvl);
log_level_is_enabled(log_level lvl);
```

### Example

```c
log_set_color_output(true);
log_enable_level(LOG_LEVEL_TRACE);
log_disable_level(LOG_LEVEL_DEBUG);

if (log_level_is_enabled(LOG_LEVEL_TRACE)) {
    TRACE("Trace is enabled!");
}
```

---

# Environment Variable: `LOG_LEVELS`

Control verbosity at runtime:

```sh
LOG_LEVELS=+DEBUG,-TRACE ./your_app
```

### Syntax

| Token             | Meaning               |
|-------------------|-----------------------|
| `ALL`             | Enable all levels     |
| `NONE`            | Disable all levels    |
| `+LEVEL`          | Enable a level        |
| `-LEVEL`          | Disable a level       |
| comma-separated   | Multiple operations   |
| case-insensitive  | `debug` == `DEBUG`    |

### Examples

```sh
LOG_LEVELS=ALL ./app
LOG_LEVELS=-debug,-trace ./app
LOG_LEVELS=+warn,+error ./app
LOG_LEVELS=NONE,+fatal ./app
```

---

# Assertions

```c
ASSERT(ptr != NULL, "ptr must not be NULL (id=%d)", id);
BUILD_ASSERT(sizeof(Header) == 64, "Header must be 64 bytes");
```

---

# Example Makefile (project using BlackBox)

```makefile
# Example Makefile For BlackBox
CC      = clang
LDFLAGS = -lblackbox

SRC = example.c
DIR = bin
OUT = test
BIN = $(DIR)/$(OUT)

LOG_LEVELS ?= ALL

all: $(BIN)
$(BIN): $(SRC)
	@mkdir -p $(DIR)
	@echo "Building example -> $(BIN)"
	@$(CC) $^ $(LDFLAGS) -o $@
	@echo
	@echo "Now run:"
	@echo " - make run "
	@echo " - make run-info "
	@echo " - make run-debug "
	@echo " - make run-fatal "
	@echo
	@echo "And see without re-compiling, levels are run-time controlled"
run: $(BIN)
	@echo "Running with LOG_LEVELS='$(LOG_LEVELS)'"
	@LOG_LEVELS="$(LOG_LEVELS)" ./$(BIN)

run-info:  ;  @$(MAKE) run LOG_LEVELS=+info,+warn
run-debug: ;  @$(MAKE) run LOG_LEVELS=+debug,-trace
run-fatal: ;  @$(MAKE) run LOG_LEVELS=+fatal

clean:
	@echo "Cleaning bin/"
	@rm -f $(BIN)
	@rmdir $(DIR) 2>/dev/null || true

.PHONY: all run run-info run-debug run-fatal clean
```

---

# Color Behavior

- `LOG_COLORS` enables ANSI colors only when `stdout` is a TTY
- File logs never contain color escape codes
- `log_set_color_output()` overrides auto-detection

---

# Platform

- Target: **macOS**
- Depends on:
  - `pthread`
  - `unistd.h`
  - `sys/stat.h`
  - `libgen.h`
  - `mach-o/dyld.h`

> Not cross-platform as-is.
> A Linux/Windows port would require replacing Mach-O path handling and parts of the filesystem logic.

---

# License

BlackBox is released under **The Unlicense** — dedicated to the public domain.
