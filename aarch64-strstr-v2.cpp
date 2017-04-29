size_t FORCE_INLINE aarch64_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    const uint8x16_t first = vdupq_n_u8(needle[0]);
    const uint8x16_t last  = vdupq_n_u8(needle[k - 1]);

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(s);

    for (size_t i = 0; i < n; i += 16) {

        const uint8x16_t block_first = vld1q_u8(ptr + i);
        const uint8x16_t block_last  = vld1q_u8(ptr + i + k - 1);

        const uint8x16_t eq_first = vceqq_u8(first, block_first);
        const uint8x16_t eq_last  = vceqq_u8(last, block_last);
        const uint8x16_t pred_16  = vandq_u8(eq_first, eq_last);

        uint64_t mask;

        mask = vgetq_lane_u64(vreinterpretq_u64_u8(pred_16), 0);
        if (mask) {
            for (int j=0; j < 8; j++) {
                if ((mask & 0xff) && (memcmp(s + i + j + 1, needle + 1, k - 2) == 0)) {
                    return i + j;
                }

                mask >>= 8;
            }
        }

        mask = vgetq_lane_u64(vreinterpretq_u64_u8(pred_16), 1);
        if (mask) {
            for (int j=0; j < 8; j++) {
                if ((mask & 0xff) && (memcmp(s + i + j + 8 + 1, needle + 1, k - 2) == 0)) {
                    return i + j + 8;
                }

                mask >>= 8;
            }
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

template <size_t k, typename MEMCMP>
size_t FORCE_INLINE aarch64_strstr_memcmp(const char* s, size_t n, const char* needle, MEMCMP memcmp_fun) {

    assert(k > 0);
    assert(n > 0);

    const uint8x16_t first = vdupq_n_u8(needle[0]);
    const uint8x16_t last  = vdupq_n_u8(needle[k - 1]);

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(s);

    for (size_t i = 0; i < n; i += 16) {

        const uint8x16_t block_first = vld1q_u8(ptr + i);
        const uint8x16_t block_last  = vld1q_u8(ptr + i + k - 1);

        const uint8x16_t eq_first = vceqq_u8(first, block_first);
        const uint8x16_t eq_last  = vceqq_u8(last, block_last);
        const uint8x16_t pred_16  = vandq_u8(eq_first, eq_last);

        uint64_t mask;
        int j;

        mask = vgetq_lane_u64(vreinterpretq_u64_u8(pred_16), 0);
        j = 0;
        while (mask) {
            if ((mask & 0xff) && (memcmp_fun(s + i + j + 1, needle + 1))) {
                return i + j;
            }

            mask >>= 8;
            j += 1;
        }

        mask = vgetq_lane_u64(vreinterpretq_u64_u8(pred_16), 1);
        j = 0;
        while (mask) {
            if ((mask & 0xff) && (memcmp_fun(s + i + j + 8 + 1, needle + 1))) {
                return i + j + 8;
            }

            mask >>= 8;
            j += 1;
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t aarch64_strstr_v2(const char* s, size_t n, const char* needle, size_t k) {

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
            result = aarch64_strstr_memcmp<2>(s, n, needle, always_true);
            break;

        case 3:
            result = aarch64_strstr_memcmp<3>(s, n, needle, memcmp1);
            break;

        case 4:
            result = aarch64_strstr_memcmp<4>(s, n, needle, memcmp2);
            break;

        case 5:
            result = aarch64_strstr_memcmp<5>(s, n, needle, memcmp4);
            break;

        case 6:
            result = aarch64_strstr_memcmp<6>(s, n, needle, memcmp4);
            break;

        case 7:
            result = aarch64_strstr_memcmp<7>(s, n, needle, memcmp5);
            break;

        case 8:
            result = aarch64_strstr_memcmp<8>(s, n, needle, memcmp6);
            break;

        case 9:
            result = aarch64_strstr_memcmp<9>(s, n, needle, memcmp8);
            break;

        case 10:
            result = aarch64_strstr_memcmp<10>(s, n, needle, memcmp8);
            break;

        case 11:
            result = aarch64_strstr_memcmp<11>(s, n, needle, memcmp9);
            break;

        case 12:
            result = aarch64_strstr_memcmp<12>(s, n, needle, memcmp10);
            break;

		default:
			result = aarch64_strstr_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}

// ------------------------------------------------------------------------

size_t aarch64_strstr_v2(const std::string& s, const std::string& needle) {

    return aarch64_strstr_v2(s.data(), s.size(), needle.data(), needle.size());
}


