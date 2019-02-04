#if defined(_MSC_VER) && (_MSC_VER < 1900)
#ifndef BT8XXEMU_INTTYPES_DEFINED_BASE
#define BT8XXEMU_INTTYPES_DEFINED_BASE
typedef unsigned __int8 uint8_t;
typedef signed __int8 int8_t;
typedef unsigned __int16 uint16_t;
typedef signed __int16 int16_t;
typedef unsigned __int32 uint32_t;
typedef signed __int32 int32_t;
typedef unsigned __int64 uint64_t;
typedef signed __int64 int64_t;
#endif
#else
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#ifdef __cplusplus
#ifdef NULL
#undef NULL
#endif
#define BT8XXEMU_INTTYPES_DEFINED_NULL
#define NULL nullptr
#endif
#endif
#ifndef BT8XXEMU_INTTYPES_DEFINED_COLOR
#define BT8XXEMU_INTTYPES_DEFINED_COLOR
typedef uint32_t argb8888;
#endif
#ifndef BT8XXEMU_INTTYPES_DEFINED_FORCEINLINE
#define BT8XXEMU_INTTYPES_DEFINED_FORCEINLINE
#ifdef _MSC_VER
#	define BT8XXEMU_FORCE_INLINE __forceinline
#else
#	define BT8XXEMU_FORCE_INLINE inline __attribute__((always_inline))
#endif
#endif
