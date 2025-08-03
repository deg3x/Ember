#ifndef ENGINE_DEFS_H
#define ENGINE_DEFS_H

typedef uint8_t  u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef uint64_t u64_t;

typedef int8_t  i8_t;
typedef int16_t i16_t;
typedef int32_t i32_t;
typedef int64_t i64_t;

typedef i8_t   b8_t;
typedef i16_t  b16_t;
typedef i32_t  b32_t;
typedef i64_t  b64_t;

typedef float  f32_t;
typedef double f64_t;

#define TRUE  1
#define FALSE 0

#define KB(n) (((u64_t)n) << 10)
#define MB(n) (((u64_t)n) << 20)
#define GB(n) (((u64_t)n) << 30)
#define TB(n) (((u64_t)n) << 40)

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))

#if defined(_MSC_VER)
    #define ALIGN_OF(t) __alignof(t)
#else
    #define ALIGN_OF(t) MAX(sizeof(t), 8)
#endif

#define global   static
#define internal static

#define ARENA_FREE_LIST 1

#if defined(_MSC_VER)
    #define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__gcc__)
    #define DEBUG_BREAK() __builtin_trap()
#else
    #define DEBUG_BREAK()
#endif

#if EMBER_ASSERT_ENABLED
void assert_fail(const char* expression, const char* message, const char* file, i32_t line)
{
    fprintf(stderr, "Assertion failed: %s [%s::%d]\n\t%s\n", expression, file, line, message);
}

#define EMBER_ASSERT(expr)                                  \
        {                                                   \
            if (expr) {}                                    \
            else                                            \
            {                                               \
                assert_fail(#expr, "", __FILE__, __LINE__); \
                DEBUG_BREAK();                              \
            }                                               \
        }

#define EMBER_ASSERT_MSG(expr, msg)                          \
        {                                                    \
            if (expr) {}                                     \
            else                                             \
            {                                                \
                assert_fail(#expr, msg, __FILE__, __LINE__); \
                DEBUG_BREAK();                               \
            }                                                \
        }

#else
#define EMBER_ASSERT(expr)
#define EMBER_ASSERT_MSG(expr, msg)
#endif

#endif // ENGINE_DEFS_H
