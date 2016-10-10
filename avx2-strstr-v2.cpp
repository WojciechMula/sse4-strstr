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


template <size_t K>
size_t FORCE_INLINE avx2_strstr_eq(const char* s, size_t n, const char* needle) {

    static_assert(K > 0 && K < 16, "K must be in range [1..15]");
    assert(n > 0);

    __m256i broadcasted[K];
    for (unsigned i=0; i < K; i++) {
        broadcasted[i] = _mm256_set1_epi8(needle[i]);
    }

    __m256i curr = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s));

    for (size_t i = 0; i < n; i += 32) {

        const __m256i next = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s + i + 32));

        __m256i eq = _mm256_cmpeq_epi8(curr, broadcasted[0]);

        // AVX2 palignr works on 128-bit lanes, thus some extra work is needed
        //
        // curr = [a, b] (2 x 128 bit)
        // next = [c, d]
        // substring = [palignr(b, a, i), palignr(c, b, i)]
        __m256i next1;
        next1 = _mm256_inserti128_si256(next1, _mm256_extracti128_si256(curr, 1), 0); // b
        next1 = _mm256_inserti128_si256(next1, _mm256_extracti128_si256(next, 0), 1); // c

        for (unsigned i=1; i < K; i++) {
            const __m256i substring = _mm256_alignr_epi8(next1, curr, i);
            eq = _mm256_and_si256(eq, _mm256_cmpeq_epi8(substring, broadcasted[i]));
        }

        curr = next;

        const uint32_t mask = _mm256_movemask_epi8(eq);
        if (mask != 0) {
            return i + bits::get_first_bit_set(mask);
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

        case 2:
            result = avx2_strstr_eq<2>(s, n, needle);
            break;

        case 3:
            result = avx2_strstr_eq<3>(s, n, needle);
            break;

        case 4:
            result = avx2_strstr_eq<4>(s, n, needle);
            break;

        case 5:
            result = avx2_strstr_eq<5>(s, n, needle);
            break;

        case 6:
            result = avx2_strstr_eq<6>(s, n, needle);
            break;

        case 7:
            result = avx2_strstr_eq<7>(s, n, needle);
            break;

        case 8:
            result = avx2_strstr_eq<8>(s, n, needle);
            break;

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


