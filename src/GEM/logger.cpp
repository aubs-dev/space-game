#include "pch.hpp"

#include "GEM/logger.hpp"

#include "GEM/core/memory.hpp"

namespace logger {
    Level thresholdLevel = Level::ALL;

    void set_threshold_level(Level level) {
        thresholdLevel = level;
    }

    void logf(Level level, const char* tag, const char* fmt, ...) {
        if (level < thresholdLevel) return;

        u64 len = snprintf(0, 0, "%s%s\n", tag, fmt) + 1; // length of: tag + format + \n + \0
        char* combinedFmt = (char*)memory::alloc(len);
        if (!combinedFmt) return;

        snprintf(combinedFmt, len, "%s%s\n", tag, fmt);

        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, combinedFmt, args);
        va_end(args);

        memory::free(combinedFmt);
    }
}
