// implements scheme described in http://0x80.pl/articles/simd-friendly-karp-rabin.html

__mmask16 FORCE_INLINE zero_byte_mask(const __m512i v) {

    const __m512i v01  = _mm512_set1_epi8(0x01);
    const __m512i v80  = _mm512_set1_epi8(int8_t(0x80));

    const __m512i v1   = _mm512_sub_epi32(v, v01);
    // tmp1 = (v - 0x01010101) & ~v & 0x80808080
    const __m512i tmp1 = _mm512_ternarylogic_epi32(v1, v, v80, 0x20);

    return _mm512_test_epi32_mask(tmp1, tmp1);
}


size_t avx512f_strstr_v2_anysize(const char* string, size_t n, const char* needle, size_t k) {

    assert(n > 0);
    assert(k > 0);

    const __m512i first = _mm512_set1_epi8(needle[0]);
    const __m512i last  = _mm512_set1_epi8(needle[k - 1]);

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

            if (memcmp(haystack + 4*p + 0, needle, k) == 0) {
                return (haystack - string) + 4*p + 0;
            }

            if (memcmp(haystack + 4*p + 1, needle, k) == 0) {
                return (haystack - string) + 4*p + 1;
            }

            if (memcmp(haystack + 4*p + 2, needle, k) == 0) {
                return (haystack - string) + 4*p + 2;
            }

            if (memcmp(haystack + 4*p + 3, needle, k) == 0) {
                return (haystack - string) + 4*p + 3;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return size_t(-1);
}


template <size_t k, typename MEMCMP>
size_t avx512f_strstr_v2_memcmp(const char* string, size_t n, const char* needle, MEMCMP memeq_fun) {

    assert(n > 0);
    assert(k > 0);

    const __m512i first = _mm512_set1_epi8(needle[0]);
    const __m512i last  = _mm512_set1_epi8(needle[k - 1]);

    char* haystack = const_cast<char*>(string);
    char* end      = haystack + n;

    for (/**/; haystack < end; haystack += 64) {

        const __m512i block_first = _mm512_loadu_si512(haystack + 0);
        const __m512i block_last  = _mm512_loadu_si512(haystack + k - 1);

        const __m512i first_zeros = _mm512_xor_si512(block_first, first);
        const __m512i zeros       = _mm512_ternarylogic_epi32(first_zeros, block_last, last, 0xf6);

        uint32_t mask = zero_byte_mask(zeros);
        while (mask) {

            const uint64_t p = __builtin_ctz(mask);

            if (memeq_fun(haystack + 4*p + 0, needle)) {
                return (haystack - string) + 4*p + 0;
            }

            if (memeq_fun(haystack + 4*p + 1, needle)) {
                return (haystack - string) + 4*p + 1;
            }

            if (memeq_fun(haystack + 4*p + 2, needle)) {
                return (haystack - string) + 4*p + 2;
            }

            if (memeq_fun(haystack + 4*p + 3, needle)) {
                return (haystack - string) + 4*p + 3;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return size_t(-1);
}

// ------------------------------------------------------------------------

size_t avx512f_strstr_v2(const char* s, size_t n, const char* needle, size_t k) {

    size_t result = std::string::npos;

    if (n < k) {
        return result;
    }

	switch (k) {
		case 0:
			return 0;

		case 1: {
            const char* res = reinterpret_cast<const char*>(strchr(s, needle[0]));

			return (res != nullptr) ? res - s : std::string::npos;
            }

        case 2:
            result = avx512f_strstr_v2_memcmp<2>(s, n, needle, memcmp2);
            break;

        case 3:
            result = avx512f_strstr_v2_memcmp<3>(s, n, needle, memcmp3);
            break;

        case 4:
            result = avx512f_strstr_v2_memcmp<4>(s, n, needle, memcmp4);
            break;

        case 5:
            result = avx512f_strstr_v2_memcmp<5>(s, n, needle, memcmp5);
            break;

        case 6:
            result = avx512f_strstr_v2_memcmp<6>(s, n, needle, memcmp6);
            break;

        case 7:
            result = avx512f_strstr_v2_memcmp<7>(s, n, needle, memcmp7);
            break;

        case 8:
            result = avx512f_strstr_v2_memcmp<8>(s, n, needle, memcmp8);
            break;

        case 9:
            result = avx512f_strstr_v2_memcmp<9>(s, n, needle, memcmp9);
            break;

        case 10:
            result = avx512f_strstr_v2_memcmp<10>(s, n, needle, memcmp10);
            break;

        case 11:
            result = avx512f_strstr_v2_memcmp<11>(s, n, needle, memcmp11);
            break;

        case 12:
            result = avx512f_strstr_v2_memcmp<12>(s, n, needle, memcmp12);
            break;

		default:
			result = avx512f_strstr_v2_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// --------------------------------------------------

size_t avx512f_strstr_v2(const std::string& s, const std::string& needle) {

    return avx512f_strstr_v2(s.data(), s.size(), needle.data(), needle.size());
}

