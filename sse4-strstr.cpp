size_t sse4_strstr_long(const char* s, size_t n, const char* neddle, size_t neddle_size) {
    
    assert(neddle_size > 4);
    assert(n > 0);

    const __m128i prefix = _mm_loadu_si128(reinterpret_cast<const __m128i*>(neddle));
    const __m128i zeros  = _mm_setzero_si128();

    for (size_t i = 0; i < n; i += 8) {
        
        const __m128i data   = _mm_loadu_si128(reinterpret_cast<const __m128i*>(s + i));
        const __m128i result = _mm_mpsadbw_epu8(data, prefix, 0);

        const __m128i cmp    = _mm_cmpeq_epi16(result, zeros);

        unsigned mask = _mm_movemask_epi8(cmp) & 0x5555;

        while (mask != 0) {

            const auto bitpos = bits::get_first_bit_set(mask)/2;

            if (memcmp(s + i + bitpos + 4, neddle + 4, neddle_size - 4) == 0) {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t sse4_strstr_max20(const char* s, size_t n, const char* neddle, size_t neddle_size) {
    
    const __m128i zeros  = _mm_setzero_si128();
    const __m128i prefix = sse::load(neddle);
    const __m128i suffix = sse::load(neddle + 4);
    const __m128i suff_mask = sse::mask_lower_bytes(neddle_size - 4);

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

size_t sse4_strstr_max36(const char* s, size_t n, const char* neddle, size_t neddle_size) {
    
    const __m128i zeros     = _mm_setzero_si128();
    const __m128i prefix    = sse::load(neddle);
    const __m128i suffix1   = sse::load(neddle + 4);
    const __m128i suffix2   = sse::load(neddle + 16 + 4);
    const __m128i suff_mask = sse::mask_higher_bytes(neddle_size - (16 + 4));

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

size_t sse4_strstr_len3(const char* s, size_t n, const char* neddle) {

    const __m128i prefix = _mm_loadu_si128(reinterpret_cast<const __m128i*>(neddle));
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

size_t sse4_strstr_len4(const char* s, size_t n, const char* neddle) {

    const __m128i prefix = _mm_loadu_si128(reinterpret_cast<const __m128i*>(neddle));
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

size_t sse4_strstr(const char* s, size_t n, const char* neddle, size_t neddle_size) {

    size_t result = std::string::npos;

    if (n < neddle_size) {
        return result;
    }

	switch (neddle_size) {
		case 0:
			return 0;

		case 1: {
            const char* res = reinterpret_cast<const char*>(strchr(s, neddle[0]));

			return (res != nullptr) ? res - s : std::string::npos;
            }
		case 2: {
			const char* res = reinterpret_cast<const char*>(strstr(s, neddle));

			return (res != nullptr) ? res - s : std::string::npos;
            }
		case 3:

			result = sse4_strstr_len3(s, n, neddle);
            break;

		case 4:
			result = sse4_strstr_len4(s, n, neddle);
            break;

		case 5: case 6: case 7: case 8: case 9:
		case 10: case 11: case 12: case 13: case 14:
		case 15: case 16: case 17: case 18: case 19:
		case 20: /* 5..20 */
		    result = sse4_strstr_max20(s, n, neddle, neddle_size);
            break;

		case 21: case 22: case 23: case 24: case 25: 
		case 26: case 27: case 28: case 29: case 30: 
		case 31: case 32: case 33: case 34: case 35: 
		case 36: /* 21..36 */
			result = sse4_strstr_max36(s, n, neddle, neddle_size);
            break;

		default:
			result = sse4_strstr_long(s, n, neddle, neddle_size);
            break;
    }


    if (result <= n - neddle_size) {
        return result;
    } else {
        return std::string::npos;
    }
}

// --------------------------------------------------

size_t sse4_strstr(const std::string& s, const std::string& neddle) {

    return sse4_strstr(s.data(), s.size(), neddle.data(), neddle.size());
}

