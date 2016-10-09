// implements scheme described in http://0x80.pl/articles/simd-friendly-karp-rabin.html

size_t FORCE_INLINE avx2_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    const __m256i first = _mm256_set1_epi8(needle[0]);
    const __m256i last  = _mm256_set1_epi8(needle[k - 1]);

    for (size_t i = 0; i < n; i += 32) {

        const __m256i block_first = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s + i));
        const __m256i block_last  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s + i + k - 1));

        const __m256i eq_first = _mm256_cmpeq_epi8(first, block_first);
        const __m256i eq_last  = _mm256_cmpeq_epi8(last, block_last);

        uint32_t mask = _mm256_movemask_epi8(_mm256_and_si256(eq_first, eq_last));

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

size_t avx2_strstr_v2(const char* s, size_t n, const char* needle, size_t k) {

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
			result = avx2_strstr_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t avx2_strstr_v2(const std::string& s, const std::string& needle) {

    return avx2_strstr_v2(s.data(), s.size(), needle.data(), needle.size());
}


