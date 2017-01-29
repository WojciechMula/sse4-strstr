#pragma once

#define FORCE_INLINE inline __attribute__((always_inline))
#define MAYBE_UNUSED inline __attribute__((unused))

#if defined(HAVE_NEON_INSTRUCTIONS)
#   include <arm_neon.h>
#   define USE_SIMPLE_MEMCMP // for fixed-memcmp.cpp
#else
#   include <immintrin.h>
#endif
