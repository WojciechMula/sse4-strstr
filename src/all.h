#pragma once

#include "common.h"
#include <utils/bits.cpp>
#include <errno.h>
#include "fixed-memcmp.cpp"
#include "scalar.cpp"
#include "swar64-strstr-v2.cpp"
#include "swar32-strstr-v2.cpp"
#ifdef HAVE_SSE_INSTRUCTIONS
#   include <utils/sse.cpp>
#   include "sse4-strstr.cpp"
#   include "sse4-strstr-unrolled.cpp"
#   include "sse4.2-strstr.cpp"
#   include "sse2-strstr.cpp"
#   include "sse-naive-strstr.cpp"
#   include "sse2-needle4.cpp"
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
#   include <utils/avx2.cpp>
#   include "avx2-strstr.cpp"
#   include "avx2-strstr-v2.cpp"
#   include "avx2-naive-strstr.cpp"
#   include "avx2-naive-strstr64.cpp"
#   include "avx2-naive-unrolled-strstr.cpp"
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
#   include "avx512f-strstr.cpp"
#   include "avx512f-strstr-v2.cpp"
#endif
#ifdef HAVE_AVX512BW_INSTRUCTIONS
#   include "avx512bw-strstr-v2.cpp"
#   include "avx512bw-strstr-v3.cpp"
#endif
#ifdef HAVE_NEON_INSTRUCTIONS
#   include <utils/neon.cpp>
#   include "neon-strstr-v2.cpp"
#endif
#ifdef HAVE_AARCH64_ARCHITECTURE
#   include "aarch64-strstr-v2.cpp"
#endif

