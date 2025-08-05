#include <float.h>
#include <stdint.h>

// -------------------------------------------
// Primitive Types
// -------------------------------------------

// Unsigned integer types
using u8  = uint8_t;  // unsigned char
using u16 = uint16_t; // unsigned short
using u32 = uint32_t; // unsigned long
using u64 = uint64_t; // unsigned long long

const u8 U8_MAX = UINT8_MAX;
const u16 U16_MAX = UINT16_MAX;
const u32 U32_MAX = UINT32_MAX;
const u64 U64_MAX = UINT64_MAX;

// Signed integer types
using i8  = int8_t;  // signed char
using i16 = int16_t; // signed short
using i32 = int32_t; // signed long
using i64 = int64_t; // signed long long

const i8 I8_MAX = INT8_MAX;
const i16 I16_MAX = INT16_MAX;
const i32 I32_MAX = INT32_MAX;
const i64 I64_MAX = INT64_MAX;

// Floating point types
using f32 = float;
using f64 = double;

const f32 F32_MIN = FLT_MIN;
const f32 F32_MAX = FLT_MAX;
const f64 F64_MIN = DBL_MIN;
const f64 F64_MAX = DBL_MAX;

// Size types
using usize = size_t;
using isize = intptr_t;
