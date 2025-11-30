<p align="center">
  <img src="https://github.com/user-attachments/assets/00e305bc-7d20-4af1-a3e5-c794c76b60b5" width="600" alt="BlackBox Logo" />
</p>

# BlackBox

**BlackBox** is a fast, minimal logging library for C — inspired by aircraft flight recorders: log everything.

> [!NOTE]
> Built and tested exclusively on **macOS (POSIX)**.
> Uses `pthread`, `unistd`, and Mach-O (`_NSGetExecutablePath`) APIs.

---

## Features

- Pure C (`blackbox.h` / `blackbox.c`)
- Color-coded terminal output (ANSI-aware, auto-disabled for files)
- Runtime log level control via `LOG_LEVELS=...`
- Optional **file logging** with timestamped filenames + `logs/latest.log`
- Optional **stderr redirection** to its own log file
- Thread-safe via `pthread_mutex`
- Custom runtime and compile-time assertions (`ASSERT`, `BUILD_ASSERT`)

---

# Getting Started

## 1) Clone

```sh
git clone https://github.com/abnore/BlackBox.git
cd BlackBox
```

## 2) (Optional) Install system-wide on macOS

### Step 1 — Build the shared library (.dylib)

```sh
clang -dynamiclib -fPIC -o libblackbox.dylib blackbox.c
```

### Step 2 — Set the install name (required on macOS)

```sh
sudo install_name_tool -id /usr/local/lib/libblackbox.dylib libblackbox.dylib
```

### Step 3 — Install the header and library globally

```sh
sudo cp blackbox.h /usr/local/include/
sudo cp libblackbox.dylib /usr/local/lib/
```

### Uninstall

```sh
sudo rm /usr/local/include/blackbox.h
sudo rm /usr/local/lib/libblackbox.dylib
```

---

# Usage (Full Example)

```c
#include <blackbox.h>
// #include "blackbox.h"  // if using locally in your project

int main(void) {
    // Log only to the terminal (color enabled if supported)
    init_log(NO_LOG, LOG_COLORS, STDERR_TO_TERMINAL);

    // Example: write logs to files instead of the terminal
    // init_log(LOG, NO_COLORS, STDERR_TO_TERMINAL);

    // Example: redirect stderr to its own file (error-*.log)
    // init_log(LOG, NO_COLORS, STDERR_TO_LOG);

    FATAL("Unrecoverable error; program will abort after this");
    ERROR("Recoverable error; operation failed");
    WARN("Non-fatal anomaly");
    INFO("General status / progress");
    DEBUG("Developer debug information (x=%d)", 42);
    TRACE("Verbose internal tracing");

    // Example assertion (this WILL abort)
    // ASSERT(0 == 1, "This will abort with a FATAL log");

    shutdown_log();
    return 0;
}
```

Compile:

```sh
clang main.c -lblackbox -pthread -o main
```

> [!NOTE]
> BlackBox uses pthread mutexes internally.
> On macOS this works **even without** `-pthread` (libSystem includes pthreads),
> but **adding `-pthread` is recommended** for correctness and portability.

---

# Optional Runtime Controls

BlackBox provides small helper functions for dynamic configuration:

```c
log_set_color_output(bool enabled);     // Force-enable or disable ANSI colors
log_enable_level(log_level lvl);        // Enable a log level at runtime
log_disable_level(log_level lvl);       // Disable a log level
log_level_is_enabled(log_level lvl);    // Query if a level is enabled
```

Examples:

```c
log_set_color_output(true);            // force colors on
log_enable_level(LOG_LEVEL_TRACE);     // enable TRACE dynamically
log_disable_level(LOG_LEVEL_DEBUG);    // disable DEBUG
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

Syntax:

- `ALL` – enable all levels
- `NONE` – disable all
- `+LEVEL` / `-LEVEL` – enable or disable
- Comma-separated, case-insensitive
- First `+LEVEL` clears all levels unless ALL or NONE is used

Examples:

```sh
LOG_LEVELS=ALL ./app
LOG_LEVELS=-debug,-trace ./app
LOG_LEVELS=+info,+debug ./app
LOG_LEVELS=NONE,+fatal ./app
```

---

# Assertions

```c
// Logs a FATAL line with file/line and message, then aborts.
ASSERT(ptr != NULL, "ptr must not be NULL (id=%d)", id);

// Compile-time assertion
BUILD_ASSERT(sizeof(MyHdr) == 32, "MyHdr must be 32 bytes");
```

---

# Example Makefile
*(Optional — only if writing your own program with BlackBox installed system-wide)*

```makefile
.PHONY: all run run-info run-debug run-fatal clean

CC      = clang
CFLAGS  = -Wall -Wextra -O2 -I.
LDFLAGS = -pthread -lblackbox

SRC = src/main.c
OBJ = $(SRC:.c=.o)
OUT = bin/test

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

run-info:  ; $(MAKE) run LOG_LEVELS=+info,+warn
run-debug: ; $(MAKE) run LOG_LEVELS=+debug,-trace
run-fatal: ; $(MAKE) run LOG_LEVELS=NONE,+fatal
```

---

# Color Behavior

- `LOG_COLORS` enables ANSI colors only when stdout is a TTY
- File logs are always plain text
- `log_set_color_output(bool)` overrides automatic behavior

---

# Platform

- Target: **macOS**
- Depends on: `pthread`, `unistd`, `sys/stat`, `libgen`, `mach-o/dyld.h`

> Not cross-platform as-is.
> Linux/Windows ports require replacing Mach-O path handling and some FS logic.

---
