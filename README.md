<p align="center">
  <img src="https://github.com/user-attachments/assets/cc8d4ba4-4fbd-4985-b65f-2ecec1e54df4" width="400" alt="Canopy Logo" />
</p>

**Blackbox** is a fast, minimal logging library for C, inspired by aircraft flight recorders — log everything!

> [!NOTE]
> This library is **not** cross-platform — it is built and tested exclusively on **macOS**.

---

## Features

-  Clean, zero-dependency design (Pure C - standard library only)
-  Color-coded terminal output (ANSI-aware)
-  Runtime log level control via `LOG_LEVELS=...`
-  Optional log-to-file output
-  Custom assertion & build-time checks
-  Thread-safe output
-  Designed for debugging, safe for production

---
## Getting Started

### 1. Clone the repository

```sh
git clone https://github.com/abnore/BlackBox.git
cd BlackBox
```
### 2. Build your project

If you’re using the example Makefile:
```sh
make
```

### 3. Run with logging
```sh
make run-all      # Logs everything (default)
make run-info     # Logs only info and warnings
make run-debug    # Debug logs, trace disabled
make run-fatal    # Only critical fatal errors
```
Or run directly
```sh
LOG_LEVELS=+debug ./bin/test
```
### 4. Include in your own project
To use Blackbox in your own C project:
- Copy logger.h and logger.c into your source tree
- Include it in your code:
```c
#include "logger.h"
```
- initialize and shutdown
```c
  init_log(NULL, true);
  INFO("Logger is live!");
  shutdown_log();
```



## Example Usage

```c
#include "logger.h"

int main(void) {
    init_log(NULL, true); // log to stdout with color
    INFO("App started!");
    DEBUG("This is a debug message.");
    TRACE("This is a trace message.");
    shutdown_log();
}
```

---

## Available Log Levels

- `FATAL` – unrecoverable error, must exit
- `ERROR` – something went wrong, but can recover
- `WARN` – unexpected but non-fatal
- `INFO` – general application status
- `DEBUG` – dev-time debugging details
- `TRACE` – high-volume diagnostic detail

---
## Use with Makefile

You can configure log levels dynamically in your Makefile using the LOG_LEVELS environment variable.
This is useful if you want to run your program with different verbosity levels during development, testing, 
or release builds — without recompiling your code.
Just prefix your run command with LOG_LEVELS=... like this:

```makefile
# Compiler and flags
CC = clang
CFLAGS = -I. -Ilogger

# Files and Directories
SRC = src/main.c logger/logger.c
OBJ = $(SRC:.c=.o) $(SRC:.m=.o)
OUT = bin/test

# Default log level
LOG_LEVELS ?= ALL

# Default target
all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(OUT)

# Run the program with LOG_LEVELS
run:
	@echo "Running with LOG_LEVELS='$(LOG_LEVELS)'..."
	@LOG_LEVELS="$(LOG_LEVELS)" ./$(OUT)

# Clean build artifacts
clean:
	rm -f $(OBJ) $(OUT)

# Run with specific logging profiles
run-all:
	LOG_LEVELS=ALL make run

run-info:
	LOG_LEVELS=+info,+warn make run

run-debug:
	LOG_LEVELS=+debug,-trace make run

run-fatal:
	LOG_LEVELS=NONE,+fatal make run
```
Each target sets a different logging configuration before launching your program.
This way, you can toggle what gets logged just by choosing a different make target:

```sh
make run-info
make run-all
```
<details>
<summary><strong> Examples <code>LOG_LEVELS</code></strong></summary>

You can configure log filtering dynamically via the environment variable `LOG_LEVELS`.

### Syntax

```sh
LOG_LEVELS=+DEBUG,-TRACE ./your_app
```
You may also use:

- `ALL` – enable all levels
- `NONE` – disable all

Log level names are **case-insensitive**.

- Use `+LEVEL` to enable, `-LEVEL` to disable
- Comma-separated list: `+INFO,+DEBUG,-TRACE`
- If no `ALL` or `NONE` is used, the first explicit level disables the rest
- Mix and match freely!

### Examples
Multiple versions of commands

```sh
# Enable all levels (default)
LOG_LEVELS=ALL ./your_app

# Disables all levels, no logging
LOG_LEVELS=none ./your_app

# Enable only INFO and DEBUG
LOG_LEVELS=none,+info,+debug ./your_app
LOG_LEVELS=+info,+debug ./your_app

# Enable all except TRACE
LOG_LEVELS=ALL,-trace ./your_app
LOG_LEVELS=-trace ./your_app

# Disable all except FATAL
LOG_LEVELS=NONE,+fatal ./your_app
LOG_LEVELS=+fatal ./your_app

# Disable just DEBUG and TRACE
LOG_LEVELS=ALL,-debug,-trace ./your_app
LOG_LEVELS=-debug,-trace ./your_app
```

</details>

---

## API Overview

```c
log_type init_log(const char* filepath, bool enable_colors);
void shutdown_log(void);

void configure_log_levels_from_env(void);
void log_set_color_output(bool enabled);

void log_enable_level(log_level level);
void log_disable_level(log_level level);
bool log_level_is_enabled(log_level level);
```

---

### Assertion Macros

**Blackbox** includes custom assertion utilities to help catch bugs and misbehaviors early:

```c
ASSERT(condition, "Something went wrong");
```

- Logs a **FATAL** message with file, line, and custom message if the condition fails.
- Does **not abort** the program — lets you fail gracefully with logs.
- Useful for runtime checks during development.

You also get:

```c
BUILD_ASSERT(sizeof(my_struct) == 32, "Unexpected struct size");
```

- A **compile-time** check using `_Static_assert`
- Ideal for validating assumptions about struct layouts or constants

---

**Blackbox** — because sometimes, the logs are the last thing that survives.
Designed for clarity, speed, and practical debugging.
