<p align="center">
  <img src="https://github.com/user-attachments/assets/00e305bc-7d20-4af1-a3e5-c794c76b60b5" width="600" alt="Blackbox Logo" />
</p>

# Blackbox

**Blackbox** is a fast, minimal logging library for C — inspired by aircraft flight recorders: log everything.

> [!NOTE]
> Built and tested exclusively on **macOS (POSIX)**.
> Uses `pthread`, `unistd`, and Mach-O (`_NSGetExecutablePath`) APIs.

---

## Features

- Pure C (`blackbox.h` / `blackbox.c`)
- Color-coded terminal output (ANSI-aware, auto-disabled for files)
- Runtime log level control via `LOG_LEVELS=...`
- Optional **file logging** with timestamped filenames + `logs/latest.log` symlink
- Optional **stderr redirection** to its own log file
- Thread-safe via `pthread_mutex`
- Custom runtime and compile-time assertions (`ASSERT`, `BUILD_ASSERT`)

---

## Getting Started

### 1) Clone

```sh
git clone https://github.com/abnore/BlackBox.git
cd BlackBox
```

### 2) Build

If you’re using the example Makefile:
```sh
make
```

### 3) Run with different log levels
```sh
make run-all
make run-info
make run-debug
make run-fatal
```

Or run directly:
```sh
LOG_LEVELS=+debug ./bin/test
```

### 4) Integrate in your own project

Copy `blackbox.h` and `blackbox.c` into your source tree and include them, or follow
the install steps in `install.md` to get

```c
// #include "blackbox.h" // if included in the source tree
#include <blackbox.h>

int main(void) {
    // To stdout with colors if the terminal supports it; stderr stays on terminal
    init_log(NO_LOG, LOG_COLORS, STDERR_TO_TERMINAL);

    INFO("Logger is live!");
    DEBUG("Debug value x=%d", 42);

    // Example: log to file and redirect stderr to a separate error log file
    // init_log(LOG, NO_COLORS, STDERR_TO_LOG);

    shutdown_log();
}
```

---

## Log Levels

| Level  | Description                          |
|--------|--------------------------------------|
| FATAL  | Unrecoverable error; program aborts  |
| ERROR  | Recoverable error; operation failed  |
| WARN   | Non-fatal anomaly                    |
| INFO   | General status / progress            |
| DEBUG  | Developer debug information          |
| TRACE  | Verbose internal tracing             |

---

## Where Logs Go

When file logging is enabled (`init_log(LOG, ...)`), Blackbox writes to:

```
logs/
├── 2025-10-19_22-15-03-yourapp.log
├── latest.log                       → symlink to most recent file
└── error-2025-10-19_22-15-03.log    (if STDERR_TO_LOG)
```

The `logs/` directory is created automatically. Colors are disabled for file output.

---

## Environment Variable: `LOG_LEVELS`

Filter verbosity at runtime without recompiling.

```sh
LOG_LEVELS=+DEBUG,-TRACE ./your_app
```

**Syntax & rules**
- `ALL` – enable all levels
- `NONE` – disable all
- `+LEVEL` – enable a level
- `-LEVEL` – disable a level
- Comma-separated, case-insensitive
- If neither `ALL` nor `NONE` is used, the **first `+LEVEL` clears** all levels before enabling it.

**Examples**
```sh
LOG_LEVELS=ALL ./app                  # everything
LOG_LEVELS=-debug,-trace ./app        # all except DEBUG, TRACE
LOG_LEVELS=+info,+debug ./app         # only INFO and DEBUG
LOG_LEVELS=NONE,+fatal ./app          # only FATAL
```

---

## API Overview

```c
// Initialization and shutdown
log_type init_log(log_mode enable_log,
                  color_mode enable_colors,
                  stderr_mode stderr_behavior);
void shutdown_log(void);

// Color control (forces on/off; bypasses auto-tty detection)
void log_set_color_output(bool enabled);

// Runtime level control (bitmask)
void log_enable_level(log_level level);   // inline in header
void log_disable_level(log_level level);  // inline in header
bool log_level_is_enabled(log_level level);

// Logging macros (capture file/line/function automatically)
FATAL("message: %s", why);
ERROR("failed: %d", code);
WARN("unexpected: %s", detail);
INFO("started");
DEBUG("x=%d", x);
TRACE("loop i=%d", i);
```

---

## Assertions

```c
// Logs a FATAL line with file/line and custom message, then **aborts** the process.
ASSERT(ptr != NULL, "ptr must not be NULL (id=%d)", id);

// Compile-time check using _Static_assert
BUILD_ASSERT(sizeof(MyHdr) == 32, "MyHdr must be 32 bytes");
```

---

## Makefile (example for system installed blackbox)

```makefile
.PHONY: all run run-all run-info run-debug run-fatal clean

CC      = clang
CFLAGS  = -Wall -Wextra -O2 -I.
LDFLAGS = -pthread -lblackbox

SRC = src/main.c
OBJ = $(SRC:.c=.o)
OUT = bin/test

# Default log level mask for 'make run'
LOG_LEVELS ?= ALL

all: $(OUT)

$(OUT): $(OBJ)
	@mkdir -p $(dir $(OUT))
	$(CC) $(OBJ) $(LDFLAGS) -o $(OUT)

run:
	@echo "Running with LOG_LEVELS='$(LOG_LEVELS)'..."
	@LOG_LEVELS="$(LOG_LEVELS)" ./$(OUT)

clean:
	rm -f $(OBJ) $(OUT)

run-all:   ; $(MAKE) run LOG_LEVELS=ALL
run-info:  ; $(MAKE) run LOG_LEVELS=+info,+warn
run-debug: ; $(MAKE) run LOG_LEVELS=+debug,-trace
run-fatal: ; $(MAKE) run LOG_LEVELS=NONE,+fatal
```

---

## Color Behavior

- `LOG_COLORS` enables ANSI colors **only** if `stdout` is a TTY (`isatty`).
- File outputs are plain text (no color).
- You can force color on/off at runtime with `log_set_color_output(bool)`.

---

## Platform

- Target: **macOS**
- Depends on: `pthread`, `unistd`, `sys/stat`, `libgen`, `mach-o/dyld.h`

> Not cross-platform as-is. Linux/Windows ports would need replacements for Mach-O path discovery and minor FS bits.

---
