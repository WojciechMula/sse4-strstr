// implements scheme described in http://0x80.pl/articles/simd-friendly-karp-rabin.html

#define FORCE_INLINE inline __attribute__((always_inline))

__mmask16 FORCE_INLINE zero_byte_mask(const __m512i v) {

    const __m512i v01  = _mm512_set1_epi32(0x01010101u);
    const __m512i v80  = _mm512_set1_epi32(0x80808080u);

    const __m512i v1   = _mm512_sub_epi32(v, v01);
    const __m512i tmp1 = _mm512_ternarylogic_epi32(v1, v, v80, 0x20);

    return _mm512_test_epi32_mask(tmp1, tmp1);
}


size_t avx512f_strstr_v2(const char* string, size_t n, const char* needle, size_t k) {

    assert(n > 0);
    assert(k > 0);

#define _mm512_set1_epu8(c) _mm512_set1_epi32(uint32_t(c) * 0x01010101u)

    const __m512i first = _mm512_set1_epu8(needle[0]);
    const __m512i last  = _mm512_set1_epu8(needle[k - 1]);

    char* haystack = const_cast<char*>(string);
    char* end      = haystack + n;

    for (/**/; haystack < end; haystack += 64) {

        const __m512i block_first = _mm512_loadu_si512(haystack + 0);
        const __m512i block_last  = _mm512_loadu_si512(haystack + k - 1);
        
#if 0
        const __m512i first_zeros = _mm512_xor_si512(block_first, first);
        const __m512i last_zeros  = _mm512_xor_si512(block_last, last);
        const __m512i zeros       = _mm512_or_si512(first_zeros, last_zeros);
#else
        const __m512i first_zeros = _mm512_xor_si512(block_first, first);
        /*
            first_zeros | block_last | last |  first_zeros | (block_last ^ last)
            ------------+------------+------+------------------------------------
                 0      |      0     |   0  |      0
                 0      |      0     |   1  |      1
                 0      |      1     |   0  |      1
                 0      |      1     |   1  |      0
                 1      |      0     |   0  |      1
                 1      |      0     |   1  |      1
                 1      |      1     |   0  |      1
                 1      |      1     |   1  |      1
        */
        const __m512i zeros       = _mm512_ternarylogic_epi32(first_zeros, block_last, last, 0xf6);
#endif

        uint32_t mask = zero_byte_mask(zeros);
        while (mask) {

            const uint64_t p = __builtin_ctz(mask);

            if (memcmp(haystack + 4*p + 0, needle, k - 0) == 0) {\
                return (haystack - string) + 4*p + 0;
            }

            if (memcmp(haystack + 4*p + 1, needle, k - 0) == 0) {\
                return (haystack - string) + 4*p + 1;
            }

            if (memcmp(haystack + 4*p + 2, needle, k - 0) == 0) {\
                return (haystack - string) + 4*p + 2;
            }

            if (memcmp(haystack + 4*p + 3, needle, k - 0) == 0) {\
                return (haystack - string) + 4*p + 3;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return size_t(-1);
}

// --------------------------------------------------

size_t avx512f_strstr_v2(const std::string& s, const std::string& needle) {

    return avx512f_strstr_v2(s.data(), s.size(), needle.data(), needle.size());
}
