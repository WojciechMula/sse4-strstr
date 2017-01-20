#pragma once

#define FORCE_INLINE inline __attribute__((always_inline))
#define MAYBE_UNUSED inline __attribute__((unused))

#if defined(HAVE_NEON_INSTRUCTIONS)
#   include <arm_neon.h>
#else
#   include <immintrin.h>
#endif
