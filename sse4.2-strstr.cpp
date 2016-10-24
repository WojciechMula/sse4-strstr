/* Usage of PCMPESTRM instruction from SSE 4.1 */

size_t FORCE_INLINE sse42_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    const __m128i N = _mm_loadu_si128((__m128i*)needle);

    for (size_t i = 0; i < n; i += 16) {
    
        const int mode = _SIDD_UBYTE_OPS 
                       | _SIDD_CMP_EQUAL_ORDERED
                       | _SIDD_BIT_MASK;

        const __m128i D   = _mm_loadu_si128((__m128i*)(s + i));
        const __m128i res = _mm_cmpestrm(N, k, D, n - i, mode);
        uint64_t mask = _mm_cvtsi128_si64(res);

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask);

            // we know that at least the first character of needle matches
            if (memcmp(s + i + bitpos + 1, needle + 1, k - 1) == 0) {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}


template <size_t k, typename MEMCMP>
size_t FORCE_INLINE sse42_strstr_memcmp(const char* s, size_t n, const char* needle, MEMCMP memcmp_fun) {

    assert(k > 0);
    assert(n > 0);

    const __m128i N = _mm_loadu_si128((__m128i*)needle);

    for (size_t i = 0; i < n; i += 16) {
    
        const int mode = _SIDD_UBYTE_OPS 
                       | _SIDD_CMP_EQUAL_ORDERED
                       | _SIDD_BIT_MASK;

        const __m128i D   = _mm_loadu_si128((__m128i*)(s + i));
        const __m128i res = _mm_cmpestrm(N, k, D, n - i, mode);
        uint64_t mask = _mm_cvtsi128_si64(res);

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask);

            if (memcmp_fun(s + i + bitpos + 1, needle + 1)) {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse42_strstr(const char* s, size_t n, const char* needle, size_t k) {

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
            result = sse42_strstr_memcmp<2>(s, n, needle, memcmp1);
            break;

        case 3:
            result = sse42_strstr_memcmp<3>(s, n, needle, memcmp2);
            break;

        case 4:
            result = sse42_strstr_memcmp<4>(s, n, needle, memcmp3);
            break;

        case 5:
            result = sse42_strstr_memcmp<5>(s, n, needle, memcmp4);
            break;

        case 6:
            result = sse42_strstr_memcmp<6>(s, n, needle, memcmp5);
            break;

        case 7:
            result = sse42_strstr_memcmp<7>(s, n, needle, memcmp6);
            break;

        case 8:
            result = sse42_strstr_memcmp<8>(s, n, needle, memcmp7);
            break;

        case 9:
            result = sse42_strstr_memcmp<9>(s, n, needle, memcmp8);
            break;

        case 10:
            result = sse42_strstr_memcmp<10>(s, n, needle, memcmp9);
            break;

        case 11:
            result = sse42_strstr_memcmp<11>(s, n, needle, memcmp10);
            break;

        case 12:
            result = sse42_strstr_memcmp<12>(s, n, needle, memcmp11);
            break;

		default:
			result = sse42_strstr_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t sse42_strstr(const std::string& s, const std::string& needle) {

    return sse42_strstr(s.data(), s.size(), needle.data(), needle.size());
}



