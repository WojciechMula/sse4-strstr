// Method descibed in https://arxiv.org/pdf/1612.01506.pdf
//
// Implementation by Daniel Lemire
// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/extra/simd/substring/substring.c

size_t FORCE_INLINE avx2_naive_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    if (n == k) {
        return (memcmp(s, needle, k) == 0) ? 0 : std::string::npos;
    }

    for (size_t i = 0; i < n - k + 1; i += 32) {
        uint32_t found = 0xffffffff;
        for (size_t j = 0; (j < k) && (found != 0) ; ++j) {
            const __m256i textvector = _mm256_loadu_si256((const __m256i *)(s + i + j));
            const __m256i needlevector = _mm256_set1_epi8(needle[j]);
            uint32_t bitmask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(textvector, needlevector));
            found = found & bitmask;
        }
        if (found != 0) {
            return i + __builtin_ctz(found);
        }
    }

    return std::string::npos;
}


// ------------------------------------------------------------------------

size_t avx2_naive_strstr(const char* s, size_t n, const char* needle, size_t k) {

    size_t result = std::string::npos;

    if (n < k) {
        return result;
    }

	result = avx2_naive_strstr_anysize(s, n, needle, k);

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t avx2_naive_strstr(const std::string& s, const std::string& needle) {

    return avx2_naive_strstr(s.data(), s.size(), needle.data(), needle.size());
}


