#pragma once

#include "pch.hpp"

namespace file {
    // -------------------------------------------
    // Data Types
    // -------------------------------------------

    struct File {
        const char* path;
        void* handle;
        bool isBinary;
    };

    enum class Mode {
        READ        = 1 << 0,
        WRITE       = 1 << 1,
        APPEND      = 1 << 2,
        READ_WRITE  = READ | WRITE,
        READ_APPEND = READ | APPEND,
    };

    enum class Offset {
        SET = 0, // set the offset relative to the start of file
        CUR = 1, // get the current offset
        END = 2, // set the offset relative to the end of file (works backwards)
    };

    // -------------------------------------------
    // Functions
    // -------------------------------------------

    // Status
    GAPI bool exists(const char* path);
    GAPI i64  get_timestamp(const char* path);

    GAPI const char* get_extension(const char* filePath);
    GAPI bool        has_extension(const char* filePath, const char* extension);

    // Operation
    GAPI File open(const char* path, Mode mode, bool binary = false);
    GAPI bool close(File* f);

    GAPI i32  get_offset(const File* f);
    GAPI bool set_offset(const File* f, i32 offset, Offset offsetMethod);
    GAPI bool set_offset_start(const File* f);
    GAPI bool set_offset_end(const File* f);

    GAPI i32 get_size(const File* f);

    GAPI bool read_line(const File* f, char* buffer, i32 bufferSize, i32* outLineLength = NULL);
    GAPI bool read(const File* f, void* buffer, i32 bufferSize, i32* outBytesRead = NULL);

    GAPI bool write_line(const File* f, const char* text);
    GAPI bool write(const File* f, const void* buffer, i32 bufferSize, i32* outBytesWritten = NULL);

    // Utility
    GAPI bool remove(const char* path);
    GAPI bool rename(const char* fromPath, const char* toPath);
    GAPI bool move(const char* fromPath, const char* toPath);

    GAPI bool copy(const char* srcPath, const char* destPath);
}

// TODO: Implement
// namespace directory {
    // -------------------------------------------
    // Data Types
    // -------------------------------------------
    // ...

    // -------------------------------------------
    // Functions
    // -------------------------------------------

    // Status
    // GAPI bool exists(const char* path);
    // GAPI bool get_timestamp(const char* path);

    // Utility
    // GAPI bool create(const char* path);
    // GAPI bool remove(const char* path);
// }
