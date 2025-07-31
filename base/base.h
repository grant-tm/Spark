#pragma once

#include <stdint.h>

// ignore token used by preprocessor to identify functions
#define def 

// concise unsigned int types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// concise int types
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// concise floating point types
typedef float   f32;
typedef double  f64;

// useful functions
#define Max(value1, value2) (value1 >= value2) ? value1 : value2
#define Min(value1, value2) (value1 <= value2) ? value1 : value2

//-----------------------------------------------------------------------------
// Arena Structure
//-----------------------------------------------------------------------------
/*
struct Arena {
    u64 reserved;
    u64 committed;
    u64 used;
    void *base;
};

def void *_ArenaPush (Arena *arena, u64 size, u64 alignment, b32 clearToZero);
#define ArenaPush(arena, size) _ArenaPush(arena, size, DefaultAlignment, true)
#define ArenaPushStruct(arena, type) _ArenaPush(arena, sizeof(struct), alignof(type), true)
#define ArenaPushArray(arena, count, type) _ArenaPush(arena, (count) * sizeof(type), alignof(type[1]))

def void ArenaClear (Arena *arena);
def void ArenaRelease (Arena *arena);
*/