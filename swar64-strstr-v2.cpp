size_t FORCE_INLINE swar64_strstr_anysize(const char* s, size_t n, const char* needle, size_t k) {

    assert(k > 0);
    assert(n > 0);

    const uint64_t first = 0x0101010101010101llu * static_cast<uint8_t>(needle[0]);
    const uint64_t last  = 0x0101010101010101llu * static_cast<uint8_t>(needle[k - 1]);

    uint64_t* block_first = reinterpret_cast<uint64_t*>(const_cast<char*>(s));
    uint64_t* block_last  = reinterpret_cast<uint64_t*>(const_cast<char*>(s + k - 1));

    // 2. sequence scan
    for (auto i=0u; i < n; i+=8, block_first++, block_last++) {
        // 0 bytes in eq indicate matching chars
        const uint64_t eq = (*block_first ^ first) | (*block_last ^ last);

        // 7th bit set if lower 7 bits are zero
        const uint64_t t0 = (~eq & 0x7f7f7f7f7f7f7f7fllu) + 0x0101010101010101llu;
        // 7th bit set if 7th bit is zero
        const uint64_t t1 = (~eq & 0x8080808080808080llu);
        uint64_t zeros = t0 & t1;
        size_t j = 0;

        while (zeros) {
            if (zeros & 0x80) {
                const char* substr = reinterpret_cast<char*>(block_first) + j + 1;
                if (memcmp(substr, needle + 1, k - 2) == 0) {
                    return i + j;
                }
            }

            zeros >>= 8;
            j += 1;
        }
    }

    return std::string::npos;
}


template <size_t k, typename MEMCMP>
size_t FORCE_INLINE swar64_strstr_memcmp(const char* s, size_t n, const char* needle, MEMCMP memcmp_fun) {

    assert(n > 0);

    const uint64_t first = 0x0101010101010101llu * static_cast<uint8_t>(needle[0]);
    const uint64_t last  = 0x0101010101010101llu * static_cast<uint8_t>(needle[k - 1]);

    uint64_t* block_first = reinterpret_cast<uint64_t*>(const_cast<char*>(s));
    uint64_t* block_last  = reinterpret_cast<uint64_t*>(const_cast<char*>(s + k - 1));

    // 2. sequence scan
    for (auto i=0u; i < n; i+=8, block_first++, block_last++) {
        const uint64_t eq = (*block_first ^ first) | (*block_last ^ last);
        const uint64_t t0 = (~eq & 0x7f7f7f7f7f7f7f7fllu) + 0x0101010101010101llu;
        const uint64_t t1 = (~eq & 0x8080808080808080llu);
        uint64_t zeros = t0 & t1;
        size_t j = 0;
    
        while (zeros) {
            if (zeros & 0x80) {
                const char* substr = reinterpret_cast<char*>(block_first) + j + 1;
                if (memcmp_fun(substr, needle + 1)) {
                    return i + j;
                }
            }

            zeros >>= 8;
            j += 1;
        }
    }

    return std::string::npos;
}

// ------------------------------------------------------------------------

size_t swar64_strstr_v2(const char* s, size_t n, const char* needle, size_t k) {

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
            result = swar64_strstr_memcmp<2>(s, n, needle, always_true);
            break;

        case 3:
            result = swar64_strstr_memcmp<3>(s, n, needle, memcmp1);
            break;

        case 4:
            result = swar64_strstr_memcmp<4>(s, n, needle, memcmp2);
            break;

        case 5:
            // Note: use memcmp4 rather memcmp3, as the last character
            //       of needle is already proven to be equal
            result = swar64_strstr_memcmp<5>(s, n, needle, memcmp4);
            break;

        case 6:
            result = swar64_strstr_memcmp<6>(s, n, needle, memcmp4);
            break;

        case 7:
            result = swar64_strstr_memcmp<7>(s, n, needle, memcmp5);
            break;

        case 8:
            result = swar64_strstr_memcmp<8>(s, n, needle, memcmp6);
            break;

        case 9:
            // Note: use memcmp8 rather memcmp7 for the same reason as above.
            result = swar64_strstr_memcmp<9>(s, n, needle, memcmp8);
            break;

        case 10:
            result = swar64_strstr_memcmp<10>(s, n, needle, memcmp8);
            break;

        case 11:
            result = swar64_strstr_memcmp<11>(s, n, needle, memcmp9);
            break;

        case 12:
            result = swar64_strstr_memcmp<12>(s, n, needle, memcmp10);
            break;

		default:
			result = swar64_strstr_anysize(s, n, needle, k);
            break;
    }

    if (result <= n - k) {
        return result;
    } else {
        return std::string::npos;
    }
}


size_t swar64_strstr_v2(const std::string& s, const std::string& needle) {

    return swar64_strstr_v2(s.data(), s.size(), needle.data(), needle.size());
}
