#ifndef BLACKBOX_H
#define BLACKBOX_H


#include <stdint.h>   // uint32_t
#include <stdbool.h>  // bool
#include <stdio.h>    // FILE, needed in function signatures

#ifdef __cplusplus
extern "C" {
#endif

//========================================
// Types
//========================================
typedef enum { NO_LOG = 0, LOG = 1 } log_mode;
typedef enum { NO_COLORS = 0, LOG_COLORS = 1 } color_mode;
typedef enum { STDERR_TO_TERMINAL = 0, STDERR_TO_LOG = 1 } stderr_mode;

#define LOG_DEFAULT         NO_LOG, LOG_COLORS, STDERR_TO_TERMINAL

/**
 * @brief Defines the severity level of a log message.
 */
typedef enum {
    LOG_LEVEL_NONE  = 0,             /**< No logging */
    LOG_LEVEL_FATAL = 1 << 0,        /**< Fatal error, program cannot continue */
    LOG_LEVEL_ERROR = 1 << 1,        /**< Recoverable error, something went wrong */
    LOG_LEVEL_WARN  = 1 << 2,        /**< Warning, something unexpected but non-fatal */
    LOG_LEVEL_INFO  = 1 << 3,        /**< General informational messages */
    LOG_LEVEL_DEBUG = 1 << 4,        /**< Debugging information for developers */
    LOG_LEVEL_TRACE = 1 << 5,        /**< Fine-grained tracing details */

    LOG_LEVEL_ALL = LOG_LEVEL_FATAL |
                    LOG_LEVEL_ERROR |
                    LOG_LEVEL_WARN  |
                    LOG_LEVEL_INFO  |
                    LOG_LEVEL_DEBUG |
                    LOG_LEVEL_TRACE
} log_level;

/**
 * @brief Defines the destination type for log output.
 */
typedef enum {
    LOG_ERROR,          /**< Internal error in logger setup */
    LOG_FILE,           /**< Logging to a file */
    LOG_STDOUT,         /**< Logging to standard output */
    LOG_ALREADY_INIT    /**< init_log was already called */
} log_type;

//========================================
// Runtime Log Configuration
//========================================

/**
 * @brief Bitmask of enabled log levels.
 *
 * This is modified at runtime through the environment variable (LOG_LEVELS)
 * or manually via the API (log_enable_level, log_disable_level).
 */

extern uint32_t log_levels_enabled;


//========================================
// Logger API
//========================================

/**
 * @brief Initialize the BlackBox logging system.
 *
 * This configures where logs go (stdout or file), whether ANSI colors are used,
 * and whether stderr is redirected. The logger can only be initialized once.
 *
 * Typical default usage:
 *
 *     init_log(LOG_DEFAULT);   // expands to: NO_LOG, LOG_COLORS, STDERR_TO_TERMINAL
 *
 * Custom examples:
 *
 *     init_log(LOG, NO_COLORS, STDERR_TO_TERMINAL);  // log to file, plain text
 *     init_log(LOG, NO_COLORS, STDERR_TO_LOG);       // log to file and redirect stderr
 *
 * Parameters:
 * @param enable_log      LOG or NO_LOG
 * @param enable_colors   LOG_COLORS or NO_COLORS
 * @param stderr_behavior STDERR_TO_TERMINAL or STDERR_TO_LOG
 *
 * @return One of:
 *         LOG_STDOUT       logging to stdout
 *         LOG_FILE         logging to a file
 *         LOG_ERROR        initialization error
 *         LOG_ALREADY_INIT logger was already initialized
 */
log_type init_log(log_mode enable_log, color_mode enable_colors, stderr_mode stderr_behavior);

/**
 * @brief Gracefully shuts down the logger and closes files if needed.
 */
void shutdown_log(void);

/**
 * @brief Force-enable or disable ANSI color output.
 */
void log_set_color_output(bool enabled);

/* Runtime log level control
/* ------------------------------------------------------- */
static inline void log_enable_level(log_level level) {
    log_levels_enabled |= level;
}

static inline void log_disable_level(log_level level) {
    log_levels_enabled &= ~level;
}

static inline bool log_level_is_enabled(log_level level) {
    return (log_levels_enabled & level) != 0;
}
/* ------------------------------------------------------- */

// Log output (wrapped by macros)
void log_output_ext(log_level level, const char* file, int line, const char* func, const char* msg, ...) __attribute__((format(printf, 5, 6)));

// Macros
#define LOG(level, msg, ...) do { \
    if (log_level_is_enabled(level)) \
        log_output_ext(level, __FILE__, __LINE__, __func__, msg, ##__VA_ARGS__); \
} while(0)

#define FATAL(msg, ...)  LOG(LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
#define ERROR(msg, ...)  LOG(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#define WARN(msg, ...)   LOG(LOG_LEVEL_WARN,  msg, ##__VA_ARGS__)
#define INFO(msg, ...)   LOG(LOG_LEVEL_INFO,  msg, ##__VA_ARGS__)
#define DEBUG(msg, ...)  LOG(LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#define TRACE(msg, ...)  LOG(LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)

// Assertions
void report_assertion_failure(const char* expr_str, const char* file, int line, const char* fmt, ...);

#define ASSERT(expr, ...) \
    do { \
        if (!(expr)) { \
            report_assertion_failure(#expr, __FILE__, __LINE__, __VA_ARGS__); \
            abort(); \
        } \
    } while (0)

#define BUILD_ASSERT(cond, msg) _Static_assert(cond, msg)


#ifdef __cplusplus
}
#endif
#endif // BLACKBOX_H
