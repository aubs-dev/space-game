#pragma once

#include "pch.hpp"

namespace memory {
    GAPI void* alloc(u64 size);
    GAPI void  free(void* block);
    GAPI void* zero(void* block, u64 size);
    GAPI void* copy(void* dest, const void* src, u64 size);
    GAPI void* set(void* block, u64 size, i32 value);
}
