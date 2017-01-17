#pragma once

#define FORCE_INLINE inline __attribute__((always_inline))

#if defined(HAVE_NEON_INSTRUCTIONS)
#   include <arm_neon.h>
#else
#   include <immintrin.h>
#endif
