size_t FORCE_INLINE sse2_needle4(const char* s, size_t n, const char* needle, size_t k) {

    uint32_t u32;
    memcpy(&u32, needle, sizeof(u32));

    const __m128i v_needle = _mm_set1_epi32(u32);
    const __m128i shuffle  = _mm_setr_epi8(0, 1, 2, 3,
                                           1, 2, 3, 4,
                                           2, 3, 4, 5,
                                           3, 4, 5, 6);

    for (size_t i = 0; i < n - k + 1; i += 4) {
        // 1. load 7 bytes:
        // [abcd|efg?|????|????]
        uint64_t u64;
        memcpy(&u64, &s[i], sizeof(u64));
        const __m128i t0 = _mm_cvtsi64x_si128(u64);

        // 2. make all possible 4-byte substrings
        // [abcd|bcde|cdef|defg]
        const __m128i t1 = _mm_shuffle_epi8(shuffle, t0);

        // 3. compare the 4-byte substrings with the needle
        const __m128i t2 = _mm_cmpeq_epi32(v_needle, t1);

        const int mask = _mm_movemask_ps((__m128)t2);
        if (mask != 0) {
            return i + __builtin_clz(mask);
        }
    }

    return std::string::npos;
}


// ------------------------------------------------------------------------

size_t sse2_strstr_needle4(const char* s, size_t n, const char* needle, size_t k) {

    if (k != 4) {
        return std::string::npos;
    }

    return sse2_needle4(s, n, needle, k);
}

// ------------------------------------------------------------------------

size_t sse2_strstr_needle4(const std::string& s, const std::string& needle) {

    return sse2_strstr_needle4(s.data(), s.size(), needle.data(), needle.size());
}


size_t FORCE_INLINE sse2_needle4_v2(const char* s, size_t n, const char* needle, size_t k) {

    uint32_t u32;
    memcpy(&u32, needle, sizeof(u32));

    const __m128i v_needle = _mm_set1_epi32(u32);
    const __m128i shuffle0 = _mm_setr_epi8(0, 1, 2, 3,
                                           1, 2, 3, 4,
                                           2, 3, 4, 5,
                                           3, 4, 5, 6);
    const __m128i shuffle1 = _mm_setr_epi8(4, 5,  6,  7,
                                           5, 6,  7,  8,
                                           6, 7,  8,  9,
                                           7, 8,  9, 10);

    for (size_t i = 0; i < n - k + 1; i += 8) {
        // 1. load 15 ytes:
        // [abcd|efgh|ijkl|????]
        const __m128i input = _mm_loadu_si128((const __m128i*)(s + i));

        // 2a. make all possible 4-byte substrings
        //     lo = [abcd|bcde|cdef|defg]
        const __m128i lo = _mm_shuffle_epi8(shuffle0, input);

        //     hi = [efgh|fghi|ghij|hijk]
        const __m128i hi = _mm_shuffle_epi8(shuffle1, input);

        // 3. compare the 4-byte substrings with the needle
        const __m128i eq_lo = _mm_cmpeq_epi32(v_needle, lo);
        const __m128i eq_hi = _mm_cmpeq_epi32(v_needle, hi);

        // to perform single movemask in the main loop
        const __m128i t0 = _mm_or_si128(eq_lo, eq_hi);

        const int mask = _mm_movemask_ps((__m128)t0);
        if (mask != 0) {
            const int mask_lo = _mm_movemask_ps((__m128)eq_lo);
            if (mask_lo != 0) {
                return i + __builtin_clz(mask_lo);
            } else {
                return i + 4 + __builtin_clz(mask);
            }
        }
    }

    return std::string::npos;
}


// ------------------------------------------------------------------------

size_t sse2_strstr_needle4_v2(const char* s, size_t n, const char* needle, size_t k) {

    if (k != 4) {
        return std::string::npos;
    }

    return sse2_needle4_v2(s, n, needle, k);
}

// ------------------------------------------------------------------------

size_t sse2_strstr_needle4_v2(const std::string& s, const std::string& needle) {

    return sse2_strstr_needle4_v2(s.data(), s.size(), needle.data(), needle.size());
}


