// Note: it appears that these specialized functions do not help.
//       But I decided to left them, just in case.

// use functions/templates dealing with certain substring length
//#define ENABLE_SSE4_LENGTH_SPECIALIZATIONS

// When defined use sse4_strstr_unrolled_memcmp template,
// otherwise use just sse4_strstr_unrolled_max20 and sse4_strstr_unrolled_max36
//#define ENABLE_SSE4_MEMCMP_TEMPLATES

size_t sse4_strstr_unrolled_anysize(const char* s, size_t n, const char* needle, size_t needle_size) {

    assert(needle_size > 4);
    assert(n > 0);

    const __m128i prefix = _mm_loadu_si128(reinterpret_cast<const __m128i*>(needle));
    const __m128i zeros  = _mm_setzero_si128();

    __m128i prev = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s));
    __m128i curr;

    for (size_t i = 0; i < n; i += 16) {

        curr  = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i + 16));

        const __m128i data0   = prev;
        const __m128i data1   = _mm_alignr_epi8(curr, prev, 8);
        const __m128i result0 = _mm_mpsadbw_epu8(data0, prefix, 0);
        const __m128i result1 = _mm_mpsadbw_epu8(data1, prefix, 0);
        prev = curr;

        const __m128i result  = _mm_packus_epi16(result0, result1);
        const __m128i cmp     = _mm_cmpeq_epi8(result, zeros);

        unsigned mask = _mm_movemask_epi8(cmp);

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask);

            if (memcmp(s + i + bitpos + 4, needle + 4, needle_size - 4) == 0) {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

template <size_t k, typename MEMCMP>
size_t sse4_strstr_unrolled_memcmp(const char* s, size_t n, const char* needle, MEMCMP memcmp_fun) {

    assert(k > 4);
    assert(n > 0);

    const __m128i prefix = _mm_loadu_si128(reinterpret_cast<const __m128i*>(needle));
    const __m128i zeros  = _mm_setzero_si128();

    __m128i prev = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s));
    __m128i curr;

    for (size_t i = 0; i < n; i += 16) {

        curr  = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i + 16));

        const __m128i data0   = prev;
        const __m128i data1   = _mm_alignr_epi8(curr, prev, 8);
        const __m128i result0 = _mm_mpsadbw_epu8(data0, prefix, 0);
        const __m128i result1 = _mm_mpsadbw_epu8(data1, prefix, 0);
        prev = curr;

        const __m128i result  = _mm_packus_epi16(result0, result1);
        const __m128i cmp     = _mm_cmpeq_epi8(result, zeros);

        unsigned mask = _mm_movemask_epi8(cmp);

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask);

            if (memcmp_fun(s + i + bitpos + 4, needle + 4)) {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse4_strstr_unrolled_max20(const char* s, size_t n, const char* needle, size_t needle_size) {

    const __m128i zeros  = _mm_setzero_si128();
    const __m128i prefix = sse::load(needle);
    const __m128i suffix = sse::load(needle + 4);
    const __m128i suff_mask = sse::mask_lower_bytes(needle_size - 4);

    for (size_t i = 0; i < n; i += 8) {

        const __m128i data   = sse::load(s + i);
        const __m128i result = _mm_mpsadbw_epu8(data, prefix, 0);

        const __m128i cmp    = _mm_cmpeq_epi16(result, zeros);

        unsigned mask = _mm_movemask_epi8(cmp) & 0x5555;

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask)/2;

            const __m128i str = sse::load(s + i + bitpos + 4);
            const __m128i cmp = _mm_cmpeq_epi8(str, suffix);

            if (_mm_testc_si128(cmp, suff_mask)) {

                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse4_strstr_unrolled_max36(const char* s, size_t n, const char* needle, size_t needle_size) {

    const __m128i zeros     = _mm_setzero_si128();
    const __m128i prefix    = sse::load(needle);
    const __m128i suffix1   = sse::load(needle + 4);
    const __m128i suffix2   = sse::load(needle + 16 + 4);
    const __m128i suff_mask = sse::mask_higher_bytes(needle_size - (16 + 4));

    for (size_t i = 0; i < n; i += 8) {

        const __m128i data   = sse::load(s + i);
        const __m128i result = _mm_mpsadbw_epu8(data, prefix, 0);

        const __m128i cmp    = _mm_cmpeq_epi16(result, zeros);

        unsigned mask = _mm_movemask_epi8(cmp) & 0x5555;

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask)/2;

            const __m128i c1 = _mm_cmpeq_epi8(sse::load(s + i + bitpos + 4), suffix1);
            const __m128i c2 = _mm_cmpeq_epi8(sse::load(s + i + bitpos + 16 + 4), suffix2);

            const __m128i c3 = _mm_or_si128(c2, suff_mask);
            const __m128i tmp = _mm_and_si128(c1, c3);

            if (_mm_movemask_epi8(tmp) == 0xffff) {

                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse4_strstr_unrolled_len3(const char* s, size_t n, const char* needle) {

    const __m128i prefix = _mm_loadu_si128(reinterpret_cast<const __m128i*>(needle));
    const __m128i zeros  = _mm_setzero_si128();

    for (size_t i = 0; i < n; i += 8) {

        const __m128i data     = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i));
        const __m128i lastbyte = _mm_cvtepu8_epi16(_mm_srli_si128(data, 3));
        const __m128i result   = _mm_mpsadbw_epu8(data, prefix, 0);

        const __m128i cmp    = _mm_cmpeq_epi16(_mm_sub_epi16(result, lastbyte), zeros);

        unsigned mask = _mm_movemask_epi8(cmp) & 0x5555;

        if (mask != 0) {

            return i + bits::get_first_bit_set(mask)/2;
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse4_strstr_unrolled_len4(const char* s, size_t n, const char* needle) {

    const __m128i prefix = _mm_loadu_si128(reinterpret_cast<const __m128i*>(needle));
    const __m128i zeros  = _mm_setzero_si128();

    for (size_t i = 0; i < n; i += 8) {

        const __m128i data   = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i));
        const __m128i result = _mm_mpsadbw_epu8(data, prefix, 0);

        const __m128i cmp    = _mm_cmpeq_epi16(result, zeros);

        unsigned mask = _mm_movemask_epi8(cmp);

        if (mask != 0) {

            return i + bits::get_first_bit_set(mask)/2;
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse4_strstr_unrolled(const char* s, size_t n, const char* needle, size_t needle_size) {

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

		case 2: {
			const char* res = reinterpret_cast<const char*>(strstr(s, needle));

			return (res != nullptr) ? res - s : std::string::npos;
            }
		case 3:

			result = sse4_strstr_unrolled_len3(s, n, needle);
            break;

		case 4:
			result = sse4_strstr_unrolled_len4(s, n, needle);
            break;

#ifdef ENABLE_SSE4_LENGTH_SPECIALIZATIONS
#ifdef ENABLE_SSE4_MEMCMP_TEMPLATES
		case 5:
            result = sse4_strstr_unrolled_memcmp<5>(s, n, needle, memcmp1);
            break;

		case 6:
            result = sse4_strstr_unrolled_memcmp<6>(s, n, needle, memcmp2);
            break;

		case 7:
            result = sse4_strstr_unrolled_memcmp<7>(s, n, needle, memcmp3);
            break;

		case 8:
            result = sse4_strstr_unrolled_memcmp<8>(s, n, needle, memcmp4);
            break;

		case 9:
            result = sse4_strstr_unrolled_memcmp<9>(s, n, needle, memcmp5);
            break;

		case 10:
            result = sse4_strstr_unrolled_memcmp<10>(s, n, needle, memcmp6);
            break;

		case 11:
            result = sse4_strstr_unrolled_memcmp<11>(s, n, needle, memcmp7);
            break;

		case 12:
            result = sse4_strstr_unrolled_memcmp<12>(s, n, needle, memcmp8);
            break;

		case 13:
            result = sse4_strstr_unrolled_memcmp<13>(s, n, needle, memcmp9);
            break;

		case 14:
            result = sse4_strstr_unrolled_memcmp<14>(s, n, needle, memcmp10);
            break;
#else
        case 5: case 6: case 7: case 8:
        case 9: case 10: case 11: case 12:
        case 13: case 14: /* 5 .. 14 */
#endif // ENABLE_SSE4_MEMCMP_TEMPLATES
		case 15: case 16: case 17: case 18: case 19:
		case 20: /* 15..20 */
		    result = sse4_strstr_unrolled_max20(s, n, needle, needle_size);
            break;

		case 21: case 22: case 23: case 24: case 25:
		case 26: case 27: case 28: case 29: case 30:
		case 31: case 32: case 33: case 34: case 35:
		case 36: /* 21..36 */
			result = sse4_strstr_unrolled_max36(s, n, needle, needle_size);
            break;
#endif // ENABLE_SSE4_LENGTH_SPECIALIZATIONS
		default:
			result = sse4_strstr_unrolled_anysize(s, n, needle, needle_size);
            break;
    }


    if (result <= n - needle_size) {
        return result;
    } else {
        return std::string::npos;
    }
}

// --------------------------------------------------

size_t sse4_strstr_unrolled(const std::string& s, const std::string& needle) {

    return sse4_strstr_unrolled(s.data(), s.size(), needle.data(), needle.size());
}


