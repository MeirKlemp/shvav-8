#pragma once

#include <cstddef>

namespace shvav8 {

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using usize = size_t;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

using f32 = float;
using f64 = double;

static_assert(sizeof(u8) == 1, "Size of u8 must be 1 byte");
static_assert(sizeof(u16) == 2, "Size of u16 must be 2 byte");
static_assert(sizeof(u32) == 4, "Size of u32 must be 4 byte");
static_assert(sizeof(u64) == 8, "Size of u64 must be 8 byte");

static_assert(sizeof(i8) == 1, "Size of i8 must be 1 byte");
static_assert(sizeof(i16) == 2, "Size of i16 must be 2 byte");
static_assert(sizeof(i32) == 4, "Size of i32 must be 4 byte");
static_assert(sizeof(i64) == 8, "Size of i64 must be 8 byte");

static_assert(sizeof(f32) == 4, "Size of f32 must be 4 byte");
static_assert(sizeof(f64) == 8, "Size of f64 must be 8 byte");

}
