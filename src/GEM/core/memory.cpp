#include "pch.hpp"

#include "GEM/core/memory.hpp"

namespace memory {
    GAPI void* alloc(u64 size) {
        void* block = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        assert(block && "Failed to allocate memory!");
        return block;
    }

    GAPI void free(void* block) {
        VirtualFree(block, 0, MEM_RELEASE);
    }

    GAPI void* zero(void* block, u64 size) {
        return memset(block, 0, size);
    }

    GAPI void* copy(void* dest, const void* src, u64 size) {
        return memcpy(dest, src, size);
    }

    GAPI void* set(void* block, u64 size, i32 value) {
        return memset(block, value, size);
    }
}
