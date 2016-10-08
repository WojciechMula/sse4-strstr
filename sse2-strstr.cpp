// implements scheme described in http://0x80.pl/articles/simd-friendly-karp-rabin.html

size_t FORCE_INLINE sse2_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    const __m128i first = _mm_set1_epi8(needle[0]);
    const __m128i last  = _mm_set1_epi8(needle[k - 1]);

    for (size_t i = 0; i < n; i += 16) {

        const __m128i block_first = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i));
        const __m128i block_last  = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i + k - 1));

        const __m128i eq_first = _mm_cmpeq_epi8(first, block_first);
        const __m128i eq_last  = _mm_cmpeq_epi8(last, block_last);

        uint16_t mask = _mm_movemask_epi8(_mm_or_si128(eq_first, eq_last));

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask);

            if (memcmp(s + i + bitpos + 1, needle + 1, k - 2) == 0) {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse2_strstr_v2(const char* s, size_t n, const char* needle, size_t k) {

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

		default:
			result = sse2_strstr_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t sse2_strstr_v2(const std::string& s, const std::string& needle) {

    return sse2_strstr_v2(s.data(), s.size(), needle.data(), needle.size());
}


