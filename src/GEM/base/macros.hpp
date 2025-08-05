// -------------------------------------------
// Macros
// -------------------------------------------

// Exporting
#define GAPI __declspec(dllexport)

// Byte count
#define KiB(amount) ((amount) * 1024ULL)
#define MiB(amount) ((amount) * 1024ULL * 1024ULL)
#define GiB(amount) ((amount) * 1024ULL * 1024ULL * 1024ULL)

#define KB(amount) ((amount) * 1000ULL)
#define MB(amount) ((amount) * 1000ULL * 1000ULL)
#define GB(amount) ((amount) * 1000ULL * 1000ULL * 1000ULL)

// Enumerations
#define as_index(enum) ((u32)enum)

// Flags
#define FLAG_GET(flags, flag) ((flags & (u32)flag) == (u32)flag)
#define FLAG_SET(flags, flag, enabled) (flags = (enabled ? (flags | (u32)flag) : (flags & ~(u32)flag)))
#define FLAG_ADD(flags, flag) (flags |= (u32)flag)
#define FLAG_REMOVE(flags, flag) (flags &= ~(u32)flag)

// TODO: Move these macros elsewhere
// Strings
#define GEM_MAX_STRING_LENGTH 512
#define string_is_valid(str) ((str != NULL) && (str[0] != '\0'))
// Static array
#define array_get_count(arr) (sizeof(arr) / sizeof(arr[0]))
