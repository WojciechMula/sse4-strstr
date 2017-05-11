// Method described in https://arxiv.org/pdf/1612.01506.pdf
//
// Implementation by Daniel Lemire

size_t FORCE_INLINE avx2_naive_strstr_unrolled_anysize(const char* s, size_t n, const char* needle, size_t k) {

    // assert(n % 32 == 0); // deliberately commented out
    // todo: fix it so we can handle variable-length inputs and
    // can catch matches at the end of the data.
    for (size_t i = 0; i < n - k; i += 32) {
      uint32_t found = 0xFFFFFFFF; // 32 1-bits
      size_t j = 0;
      for (; (j + 3 < k) && (found != 0)  ; j += 4) {
        __m256i textvector1 = _mm256_loadu_si256((const __m256i *)(s + i + j));
        __m256i needlevector1 = _mm256_set1_epi8(needle[j]);
        __m256i textvector2 = _mm256_loadu_si256((const __m256i *)(s + i + j + 1));
        __m256i needlevector2 = _mm256_set1_epi8(needle[j + 1]);
        __m256i cmp1 = _mm256_cmpeq_epi8(textvector1, needlevector1);
        __m256i cmp2 = _mm256_cmpeq_epi8(textvector2, needlevector2);
        __m256i textvector3 = _mm256_loadu_si256((const __m256i *)(s + i + j + 2));
        __m256i needlevector3 = _mm256_set1_epi8(needle[j + 2]);
        __m256i textvector4 = _mm256_loadu_si256((const __m256i *)(s + i + j + 3));
        __m256i needlevector4 = _mm256_set1_epi8(needle[j + 3]);
        __m256i cmp3 = _mm256_cmpeq_epi8(textvector3, needlevector3);
        __m256i cmp4 = _mm256_cmpeq_epi8(textvector4, needlevector4);
        __m256i cmp12 = _mm256_and_si256(cmp1,cmp2);
        __m256i cmp34 = _mm256_and_si256(cmp3,cmp4);
        uint32_t bitmask = _mm256_movemask_epi8(_mm256_and_si256(cmp12,cmp34));
        found = found & bitmask;
      }
      for (; (j < k) && (found != 0) ; ++j) {
        __m256i textvector = _mm256_loadu_si256((const __m256i *)(s + i + j));
        __m256i needlevector = _mm256_set1_epi8(needle[j]);
        uint32_t bitmask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(textvector, needlevector));
        found = found & bitmask;
      }
      if(found != 0) {
        // got a match... maybe
        return i + __builtin_ctz(found);
      }
    }

    return std::string::npos;
}


// ------------------------------------------------------------------------

size_t avx2_naive_unrolled_strstr(const char* s, size_t n, const char* needle, size_t k) {

    size_t result = std::string::npos;

    if (n < k) {
        return result;
    }

	result = avx2_naive_strstr_unrolled_anysize(s, n, needle, k);

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t avx2_naive_unrolled_strstr(const std::string& s, const std::string& needle) {

    return avx2_naive_unrolled_strstr(s.data(), s.size(), needle.data(), needle.size());
}


