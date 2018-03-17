// Method descibed in https://arxiv.org/pdf/1612.01506.pdf
//
// Implementation by Daniel Lemire
// https://github.com/lemire/Code-used-on-Daniel-Lemire-s-blog/blob/master/extra/simd/substring/substring.c

size_t FORCE_INLINE sse_naive_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    if (n == k) {
        return (memcmp(s, needle, k) == 0) ? 0 : std::string::npos;
    }

    for (size_t i = 0; i < n - k + 1; i += 16) {
        uint16_t found = 0xffff;
        for (size_t j = 0; (j < k) && (found != 0) ; ++j) {
            const __m128i textvector = _mm_loadu_si128((const __m128i *)(s + i + j));
            const __m128i needlevector = _mm_set1_epi8(needle[j]);
            uint16_t bitmask = _mm_movemask_epi8(_mm_cmpeq_epi8(textvector, needlevector));
            found = found & bitmask;
        }
        if (found != 0) {
            return i + __builtin_ctz(found);
        }
    }

    return std::string::npos;
}


// ------------------------------------------------------------------------

size_t sse_naive_strstr(const char* s, size_t n, const char* needle, size_t k) {

    size_t result = std::string::npos;

    if (n < k) {
        return result;
    }

	result = sse_naive_strstr_anysize(s, n, needle, k);

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t sse_naive_strstr(const std::string& s, const std::string& needle) {

    return sse_naive_strstr(s.data(), s.size(), needle.data(), needle.size());
}


