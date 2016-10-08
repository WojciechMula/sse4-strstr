size_t avx2_strstr_long(const char* s, size_t n, const char* neddle, size_t neddle_size) {
    
    assert(neddle_size > 4);
    assert(n > 0);

    const uint32_t prefix32 = *reinterpret_cast<const uint32_t*>(neddle);
    const __m256i prefix = _mm256_set1_epi32(prefix32);
    const __m256i zeros  = _mm256_setzero_si256();

    const __m256i permute = _mm256_setr_epi32(
        0, 1, 2, 0,
        2, 3, 4, 0
    );

    for (size_t i = 0; i < n; i += 16) {
        
        const __m256i in     = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s + i));
        /*
            [00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29|30|31]
                                                       lane | boundary
            [00|01|02|03|04|05|06|07|08|09|10|11|??|??|??|??|08|09|10|11|12|13|14|15|16|17|18|19|??|??|??|??]
             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^             ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
        */
        const __m256i data   = _mm256_permutevar8x32_epi32(in, permute);
        const __m256i result = _mm256_mpsadbw_epu8(data, prefix, 0);

        const __m256i cmp    = _mm256_cmpeq_epi16(result, zeros);

        uint32_t mask = _mm256_movemask_epi8(cmp) & 0x55555555u;

        while (mask != 0) {

            const auto bitpos   = bits::get_first_bit_set(mask)/2;

            if (memcmp(s + i + bitpos + 4, neddle + 4, neddle_size - 4) == 0) {
                return i + bitpos;
            }

            mask = bits::clear_leftmost_set(mask);
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t avx2_strstr_len4(const char* s, size_t n, const char* neddle) {

    assert(n > 0);

    const uint32_t prefix32 = *reinterpret_cast<const uint32_t*>(neddle);
    const __m256i prefix = _mm256_set1_epi32(prefix32);
    const __m256i zeros  = _mm256_setzero_si256();

    const __m256i permute = _mm256_setr_epi32(
        0, 1, 2, 0,
        2, 3, 4, 0
    );

    for (size_t i = 0; i < n; i += 16) {
        
        const __m256i in     = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(s + i));
        const __m256i data   = _mm256_permutevar8x32_epi32(in, permute);
        const __m256i result = _mm256_mpsadbw_epu8(data, prefix, 0);

        const __m256i cmp    = _mm256_cmpeq_epi16(result, zeros);

        const uint32_t mask = _mm256_movemask_epi8(cmp) & 0x55555555u;

        if (mask != 0) {
            return i + bits::get_first_bit_set(mask)/2;
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t avx2_strstr(const char* s, size_t n, const char* neddle, size_t neddle_size) {

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
		case 2:
		case 3:
            {
			const char* res = reinterpret_cast<const char*>(strstr(s, neddle));

			return (res != nullptr) ? res - s : std::string::npos;
            }

		case 4:
			result = avx2_strstr_len4(s, n, neddle);
            break;

		default:
			result = avx2_strstr_long(s, n, neddle, neddle_size);
            break;
    }


    if (result <= n - neddle_size) {
        return result;
    } else {
        return std::string::npos;
    }
}

// --------------------------------------------------

size_t avx2_strstr(const std::string& s, const std::string& neddle) {

    return avx2_strstr(s.data(), s.size(), neddle.data(), neddle.size());
}

