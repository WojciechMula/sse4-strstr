// implements scheme described in http://0x80.pl/articles/simd-friendly-karp-rabin.html

size_t avx512bw_strstr_v3_anysize(const char* string, size_t n, const char* needle, size_t k) {

    assert(n > 0);
    assert(k > 0);

    const __m512i first = _mm512_set1_epi8(needle[0]);
    const __m512i last  = _mm512_set1_epi8(needle[k - 1]);

    char* haystack = const_cast<char*>(string);
    char* end      = haystack + n;

    for (/**/; haystack < end; haystack += 64) {

        const __m512i   block_first = _mm512_loadu_si512(haystack + 0);
        const __mmask64 first_eq    = _mm512_cmpeq_epi8_mask(block_first, first);

        if (first_eq == 0)
            continue;

        const __m512i block_last  = _mm512_loadu_si512(haystack + k - 1);
        uint64_t mask = _mm512_mask_cmpeq_epi8_mask(first_eq, block_last, last);

        while (mask != 0) {

            const uint64_t bitpos = bits::get_first_bit_set(mask);
            const char* s = reinterpret_cast<const char*>(haystack);

            if (memcmp(s + bitpos + 1, needle + 1, k - 2) == 0) {
                return (s - string) + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return size_t(-1);
}


template <size_t k, typename MEMCMP>
size_t avx512bw_strstr_v3_memcmp(const char* string, size_t n, const char* needle, MEMCMP memeq_fun) {

    assert(n > 0);
    assert(k > 0);

    const __m512i first = _mm512_set1_epi8(needle[0]);
    const __m512i last  = _mm512_set1_epi8(needle[k - 1]);

    char* haystack = const_cast<char*>(string);
    char* end      = haystack + n;

    for (/**/; haystack < end; haystack += 64) {

        const __m512i block_first = _mm512_loadu_si512(haystack + 0);
        const __mmask64 first_eq  = _mm512_cmpeq_epi8_mask(block_first, first);

        if (first_eq == 0)
            continue;

        const __m512i block_last  = _mm512_loadu_si512(haystack + k - 1);
        uint64_t mask = _mm512_mask_cmpeq_epi8_mask(first_eq, block_last, last);

        while (mask != 0) {

            const uint64_t bitpos = bits::get_first_bit_set(mask);
            const char* s = reinterpret_cast<const char*>(haystack);

            if (memeq_fun(s + bitpos + 1, needle + 1)) {
                return (s - string) + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return size_t(-1);
}

// ------------------------------------------------------------------------

size_t avx512bw_strstr_v3(const char* s, size_t n, const char* needle, size_t k) {

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
            result = avx512bw_strstr_v3_memcmp<2>(s, n, needle, always_true);
            break;

        case 3:
            result = avx512bw_strstr_v3_memcmp<3>(s, n, needle, memcmp1);
            break;

        case 4:
            result = avx512bw_strstr_v3_memcmp<4>(s, n, needle, memcmp2);
            break;

        case 5:
            result = avx512bw_strstr_v3_memcmp<5>(s, n, needle, memcmp3);
            break;

        case 6:
            result = avx512bw_strstr_v3_memcmp<6>(s, n, needle, memcmp4);
            break;

        case 7:
            result = avx512bw_strstr_v3_memcmp<7>(s, n, needle, memcmp5);
            break;

        case 8:
            result = avx512bw_strstr_v3_memcmp<8>(s, n, needle, memcmp6);
            break;

        case 9:
            result = avx512bw_strstr_v3_memcmp<9>(s, n, needle, memcmp7);
            break;

        case 10:
            result = avx512bw_strstr_v3_memcmp<10>(s, n, needle, memcmp8);
            break;

        case 11:
            result = avx512bw_strstr_v3_memcmp<11>(s, n, needle, memcmp9);
            break;

        case 12:
            result = avx512bw_strstr_v3_memcmp<12>(s, n, needle, memcmp10);
            break;

		default:
			result = avx512bw_strstr_v3_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// --------------------------------------------------

size_t avx512bw_strstr_v3(const std::string& s, const std::string& needle) {

    return avx512bw_strstr_v3(s.data(), s.size(), needle.data(), needle.size());
}

