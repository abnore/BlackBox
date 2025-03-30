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

```sh
# Enable all levels (default)
LOG_LEVELS=ALL ./your_app

# Enable only INFO and DEBUG
LOG_LEVELS=+info,+debug ./your_app

# Enable all except TRACE
LOG_LEVELS=ALL,-trace ./your_app

# Disable all except FATAL
LOG_LEVELS=NONE,+fatal ./your_app

# Disable just DEBUG and TRACE
LOG_LEVELS=ALL,-debug,-trace ./your_app
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
