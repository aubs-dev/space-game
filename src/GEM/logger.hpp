#pragma once

// -------------------------------------------
// ANSI Escape Sequences
// -------------------------------------------

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"
#define ANSI_ITALIC "\033[3m"
#define ANSI_UNDERLINE "\033[4m"
#define ANSI_STRIKE "\033[9m"

#define ANSI_BLACK "\033[30m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN "\033[36m"
#define ANSI_WHITE "\033[37m"

#define ANSI_BOLD_BLACK "\033[1m\033[30m"
#define ANSI_BOLD_RED "\033[1m\033[31m"
#define ANSI_BOLD_GREEN "\033[1m\033[32m"
#define ANSI_BOLD_YELLOW "\033[1m\033[33m"
#define ANSI_BOLD_BLUE "\033[1m\033[34m"
#define ANSI_BOLD_MAGENTA "\033[1m\033[35m"
#define ANSI_BOLD_CYAN "\033[1m\033[36m"
#define ANSI_BOLD_WHITE "\033[1m\033[37m"

#define ANSI_BG_BLACK "\033[40m"
#define ANSI_BG_RED "\033[41m"
#define ANSI_BG_GREEN "\033[42m"
#define ANSI_BG_YELLOW "\033[43m"
#define ANSI_BG_BLUE "\033[44m"
#define ANSI_BG_MAGENTA "\033[45m"
#define ANSI_BG_CYAN "\033[46m"
#define ANSI_BG_WHITE "\033[47m"

// -------------------------------------------
// Logging
// -------------------------------------------

// TODO: Add an optional flag to enable timestamps. E.g. [INFO][20:03:05] Some information message!
// TODO: Add option to select output location, stdout/stderr/file?
// TODO: Add function to print vectors, colors, booleans etc.

namespace logger {
    enum class Level {
        ALL = 0,
        TRACE,
        INFO,
        WARN,
        ERRO,
        NONE,
    };

    extern Level thresholdLevel;

    void set_threshold_level(Level level);
    void logf(Level level, const char* tag, const char* fmt, ...);
}

#if defined(GEM_DEBUG) || defined(GEM_FORCE_LOGGING)
    #define log_not_implemented(format, ...) logger::logf(logger::Level::ALL    , "[GEM] "  , format " not implemented!", ##__VA_ARGS__)
    #define log_format(format, ...)          logger::logf(logger::Level::ALL    , ""        , format, ##__VA_ARGS__)
    #define log_trace(format, ...)           logger::logf(logger::Level::TRACE  , "[TRACE] ", format, ##__VA_ARGS__)
    #define log_info(format, ...)            logger::logf(logger::Level::INFO   , "[INFO] " , format, ##__VA_ARGS__)
    #define log_warn(format, ...)            logger::logf(logger::Level::WARN   , "[WARN] " , format, ##__VA_ARGS__)
    #define log_error(format, ...)           logger::logf(logger::Level::ERRO   , "[ERRO] " , format, ##__VA_ARGS__)
#else
    #define log_not_implemented(format, ...) ((void)0)
    #define log_format(format, ...)          ((void)0)
    #define log_debug(format, ...)           ((void)0)
    #define log_info(format, ...)            ((void)0)
    #define log_warn(format, ...)            ((void)0)
    #define log_error(format, ...)           ((void)0)
#endif // GEM_DEBUG || GEM_FORCE_LOGGING
