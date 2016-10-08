/*
    string - pointer to the string
    n      - string length in bytes
    needle - pointer to another string
    n      - needle length in bytes
*/
size_t avx512f_strstr_long(const char* string, size_t n, const char* needle, size_t k) {

    assert(n > 0);
    assert(k > 4);

    __m512i curr;
    __m512i next;
    __m512i v0, v1, v2, v3;

    char* haystack = const_cast<char*>(string);
    char* last     = haystack + n;

    const uint32_t prf   = *(uint32_t*)needle; // the first 4 bytes of needle
    const __m512i prefix = _mm512_set1_epi32(prf);

    next = _mm512_loadu_si512(haystack);

    for (/**/; haystack < last; haystack += 64) {

        curr = next;
        next = _mm512_loadu_si512(haystack + 64);
        const __m512i shft = _mm512_alignr_epi32(next, curr, 1);

        v0 = curr;

        {
            const __m512i t1 = _mm512_srli_epi32(curr, 8);
            const __m512i t2 = _mm512_slli_epi32(shft, 24);
            v1 = _mm512_or_si512(t1, t2);
        }
        {
            const __m512i t1 = _mm512_srli_epi32(curr, 16);
            const __m512i t2 = _mm512_slli_epi32(shft, 16);
            v2 = _mm512_or_si512(t1, t2);
        }
        {
            const __m512i t1 = _mm512_srli_epi32(curr, 24);
            const __m512i t2 = _mm512_slli_epi32(shft, 8);
            v3 = _mm512_or_si512(t1, t2);
        }

        uint16_t m0 = _mm512_cmpeq_epi32_mask(v0, prefix);
        uint16_t m1 = _mm512_cmpeq_epi32_mask(v1, prefix);
        uint16_t m2 = _mm512_cmpeq_epi32_mask(v2, prefix);
        uint16_t m3 = _mm512_cmpeq_epi32_mask(v3, prefix);

        int index = 64;
        while (m0 | m1 | m2 | m3) {
            if (m0) {
                int pos = __builtin_ctz(m0) * 4 + 0;
                m0 = m0 & (m0 - 1);

                if (pos < index && memcmp(haystack + pos + 4, needle + 4, k - 4) == 0) {
                    index = pos;
                }
            }

            if (m1) {
                int pos = __builtin_ctz(m1) * 4 + 1;
                m1 = m1 & (m1 - 1);

                if (pos < index && memcmp(haystack + pos + 4, needle + 4, k - 4) == 0) {
                    index = pos;
                }
            }

            if (m2) {
                int pos = __builtin_ctz(m2) * 4 + 2;
                m2 = m2 & (m2 - 1);

                if (pos < index && memcmp(haystack + pos + 4, needle + 4, k - 4) == 0) {
                    index = pos;
                }
            }

            if (m3) {
                int pos = __builtin_ctz(m3) * 4 + 3;
                m3 = m3 & (m3 - 1);

                if (pos < index && memcmp(haystack + pos + 4, needle + 4, k - 4) == 0) {
                    index = pos;
                }
            }
        }

        if (index < 64) {
            return (haystack - string) + index;
        }
    }

    return size_t(-1);
}

// ------------------------------------------------------------------------

size_t avx512f_strstr_eq4(const char* string, size_t n, const char* needle) {

    assert(n > 0);

    __m512i curr;
    __m512i next;
    __m512i v0, v1, v2, v3;

    char* haystack = const_cast<char*>(string);
    char* last     = haystack + n;

    const uint32_t prf   = *(uint32_t*)needle; // the first 4 bytes of needle
    const __m512i prefix = _mm512_set1_epi32(prf);

    next = _mm512_loadu_si512(haystack);

    for (/**/; haystack < last; haystack += 64) {

        curr = next;
        next = _mm512_loadu_si512(haystack + 64);
        const __m512i shft = _mm512_alignr_epi32(next, curr, 1);

        v0 = curr;

        {
            const __m512i t1 = _mm512_srli_epi32(curr, 8);
            const __m512i t2 = _mm512_slli_epi32(shft, 24);
            v1 = _mm512_or_si512(t1, t2);
        }
        {
            const __m512i t1 = _mm512_srli_epi32(curr, 16);
            const __m512i t2 = _mm512_slli_epi32(shft, 16);
            v2 = _mm512_or_si512(t1, t2);
        }
        {
            const __m512i t1 = _mm512_srli_epi32(curr, 24);
            const __m512i t2 = _mm512_slli_epi32(shft, 8);
            v3 = _mm512_or_si512(t1, t2);
        }

        uint16_t m0 = _mm512_cmpeq_epi32_mask(v0, prefix);
        uint16_t m1 = _mm512_cmpeq_epi32_mask(v1, prefix);
        uint16_t m2 = _mm512_cmpeq_epi32_mask(v2, prefix);
        uint16_t m3 = _mm512_cmpeq_epi32_mask(v3, prefix);

        int index = 64;
        if (m0) {
            int pos = __builtin_ctz(m0) * 4 + 0;
            if (pos < index) {
                index = pos;
            }
        }

        if (m1) {
            int pos = __builtin_ctz(m1) * 4 + 1;
            if (pos < index) {
                index = pos;
            }
        }

        if (m2) {
            int pos = __builtin_ctz(m2) * 4 + 2;
            if (pos < index) {
                index = pos;
            }
        }

        if (m3) {
            int pos = __builtin_ctz(m3) * 4 + 3;
            if (pos < index) {
                index = pos;
            }
        }

        if (index < 64) {
            return (haystack - string) + index;
        }

        assert(m0 == 0 && m1 == 0 && m2 == 0 && m3 == 0);
    }

    return size_t(-1);
}

// ------------------------------------------------------------------------

size_t avx512f_strstr(const char* s, size_t n, const char* needle, size_t needle_size) {

    size_t result = std::string::npos;

    if (n < needle_size) {
        return result;
    }

	switch (needle_size) {
		case 0:
			return 0;

		case 1: {
            const char* res = reinterpret_cast<const char*>(strchr(s, needle[0]));

			return (res != nullptr) ? res - s : std::string::npos;
            }
		case 2:
		case 3: {
			const char* res = reinterpret_cast<const char*>(strstr(s, needle));

			return (res != nullptr) ? res - s : std::string::npos;
            }

		case 4:
			result = avx512f_strstr_eq4(s, n, needle);
            break;

		default:
			result = avx512f_strstr_long(s, n, needle, needle_size);
            break;
    }


    if (result <= n - needle_size) {
        return result;
    } else {
        return std::string::npos;
    }
}

// --------------------------------------------------

size_t avx512f_strstr(const std::string& s, const std::string& needle) {

    return avx512f_strstr(s.data(), s.size(), needle.data(), needle.size());
}
