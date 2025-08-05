#include "pch.hpp"

#include "GEM/core/filesystem.hpp"

#include "GEM/logger.hpp"
#include "GEM/core/memory.hpp"

namespace file {
    // -------------------------------------------
    // Status
    // -------------------------------------------

    GAPI bool exists(const char* path) {
        struct stat sb;
        return stat(path, &sb) == 0;
    }

    GAPI i64 get_timestamp(const char* path) {
        struct stat sb;
        stat(path, &sb);
        return sb.st_mtime;
    }

    GAPI const char* get_extension(const char* path) {
        const char* dot = strrchr(path, '.');
        return dot ? dot : "";
    }

    GAPI bool has_extension(const char* filePath, const char* extension) {
        const char* fileExt = file::get_extension(filePath);
        return strcmp(fileExt, extension) == 0;
    }

    // -------------------------------------------
    // Operation
    // -------------------------------------------

    GAPI File open(const char* path, Mode mode, bool binary) {
        File output = {};
        output.path = path;
        output.isBinary = binary;

        const char* modeStr;
        switch (mode) {
            using enum Mode;
            case READ:
                modeStr = binary ? "rb" : "r";
                break;
            case Mode::WRITE:
                modeStr = binary ? "wb" : "w";
                break;
            case Mode::APPEND:
                modeStr = binary ? "ab" : "a";
                break;
            case Mode::READ_WRITE:
                modeStr = binary ? "w+b" : "w+";
                break;
            case Mode::READ_APPEND:
                modeStr = binary ? "a+b" : "a+";
                break;
            default:
                log_error("Invalid file mode: %i", mode);
                break;
        }

        output.handle = fopen(path, modeStr);
        if (!output.handle) log_error("Failed to open file: '%s'", path);
        return output;
    }

    GAPI bool close(File* f) {
        if (!f->handle) return false;

        fclose((FILE*)f->handle);
        f->handle = NULL;

        return true;
    }

    GAPI i32 get_offset(const File* f) {
        if (!f->handle) return -1;
        return ftell((FILE*)f->handle);
    }

    GAPI bool set_offset(const File* f, i32 offset, Offset offsetMethod) {
        if (!f->handle) return false;
        i32 status = fseek((FILE*)f->handle, offset, (i32)offsetMethod);
        return status == 0; // 0 upon success, nonzero value otherwise
    }

    GAPI bool set_offset_start(const File* f) {
        if (!f->handle) return false;
        i32 status = fseek((FILE*)f->handle, 0, SEEK_SET);
        return status == 0; // 0 upon success, nonzero value otherwise
    }

    GAPI bool set_offset_end(const File* f) {
        if (!f->handle) return false;
        i32 status = fseek((FILE*)f->handle, 0, SEEK_END);
        return status == 0; // 0 upon success, nonzero value otherwise
    }

    GAPI i32 get_size(const File* f) {
        if (!f->handle) return 0;
        FILE* handle = (FILE*)f->handle;

        i32 curOffset = ftell(handle);
        fseek(handle, 0, SEEK_END);
        i32 size = ftell(handle); // can only return a maximum size of 2GB :/
        fseek(handle, curOffset, SEEK_SET);

        return size;
    }

    GAPI bool read_line(const File* f, char* buffer, i32 bufferSize, i32* outLineLength) {
        if (f->handle && buffer && bufferSize > 0) {
            if (fgets(buffer, bufferSize, (FILE*)f->handle) != 0) {
                i32 lineLength = (i32)strlen(buffer);
                if (outLineLength != NULL) *outLineLength = lineLength;
                return true;
            }
        }

        return false;
    }

    GAPI bool read(const File* f, void* buffer, i32 bufferSize, i32* outBytesRead) {
        if (f->handle && buffer && bufferSize > 0) {
            i32 bytesRead = (i32)fread(buffer, 1, bufferSize, (FILE*)f->handle);
            if (outBytesRead != NULL) *outBytesRead = bytesRead;
            return bytesRead == bufferSize;
        }

        return false;
    }

    GAPI bool write_line(const File* f, const char* text) {
        if (!f->handle) return false;
        FILE* handle = (FILE*)f->handle;

        // Write text on a new line
        i32 curOffset = ftell(handle);

        fseek(handle, 0, SEEK_END);
        if (ftell(handle) > 0) fputc('\n', handle);
        fputs(text, handle);

        fseek(handle, curOffset, SEEK_SET);

        // Flush the stream to immediately write the data.
        return fflush(handle) == 0;
    }

    GAPI bool write(const File* f, const void* buffer, i32 bufferSize, i32* outBytesWritten) {
        if (f->handle && buffer && bufferSize > 0) {
            FILE* handle = (FILE*)f->handle;

            // Write data on a new line
            i32 curOffset = ftell(handle);

            if (!f->isBinary) {
                fseek(handle, 0, SEEK_END);
                if (ftell(handle) > 0) fputc('\n', handle);
            }

            i32 bytesWritten = (i32)fwrite(buffer, 1, bufferSize, handle);
            if (outBytesWritten != NULL) *outBytesWritten = bytesWritten;
            if (bytesWritten != bufferSize) return false;

            if (!f->isBinary) fseek(handle, curOffset, SEEK_SET);

            // Flush the stream to immediately write the data.
            return fflush(handle) == 0;
        }

        return false;
    }

    // -------------------------------------------
    // Utility
    // -------------------------------------------

    GAPI bool remove(const char* path) {
        i32 status = ::remove(path);
        if (status != 0) {
            log_error("Failed to remove file: '%s'", path);
            return false;
        }

        return true;
    }

    GAPI bool rename(const char* fromPath, const char* toPath) {
        i32 status = ::rename(fromPath, toPath);
        if (status != 0) {
            log_error("Failed to rename file from '%s' to '%s'", fromPath, toPath);
            return false;
        }

        return true;
    }

    GAPI bool move(const char* fromPath, const char* toPath) {
        i32 status = 0;

        // Remove the "to" file incase it exists
        status = ::remove(toPath);
        if (status != 0 && errno != ENOENT) {
            log_error("Failed to remove file before moving: '%s'", toPath);
            return false;
        }

        // Move the file
        status = ::rename(fromPath, toPath);
        if (status != 0) {
            if (errno == EEXIST) {
                log_warn("Tried to move file into another: '%s' to '%s'", fromPath, toPath);
                return false;
            } else {
                log_error("Failed to move file: '%s' to '%s'", fromPath, toPath);
                return false;
            }
        }

        return true;
    }

    GAPI bool copy(const char* srcPath, const char* destPath) {
        // Check if source exists
        struct stat statSrc;
        if (stat(srcPath, &statSrc) != 0) {
            log_error("Source file does not exist: %s", srcPath);
            return false;
        }

        // Copy the source file into memory
        File srcFile = file::open(srcPath, Mode::READ, true);
        i32 srcSize = file::get_size(&srcFile);
        if (srcSize == 0) return false;

        char* srcData = (char*)memory::alloc(srcSize);
        file::read(&srcFile, srcData, srcSize, NULL);
        file::close(&srcFile);

        // Write data into the destination file
        File destFile = file::open(destPath, Mode::WRITE, true);
        file::write(&destFile, srcData, srcSize, NULL);
        file::close(&destFile);

        // Free data
        memory::free(srcData);

        return true;
    }
}
