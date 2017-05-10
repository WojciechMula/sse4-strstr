#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#include "all.h"

#include <utils/ansi.cpp>

class UnitTests final {

public:
    template <typename STRSTR>
    bool run(const char* name, STRSTR strstr_function) {

        std::printf("%s... ", name);
        std::fflush(stdout);

        for (size_t size = 1; size < 64; size++) {

            const std::string neddle = "$" + std::string(size, 'x') + "#";

            for (size_t n = 0; n < 3*16; n++) {

                const std::string prefix(n, '.');

                for (size_t k = 0; k < 3*16; k++) {
                    // '.' * k + '$' + 'x' * size + '#' + '.' * k
                    
                    const std::string suffix(k, '.');
                    const std::string str = prefix + neddle + suffix;
                    
                    const auto result = strstr_function(str.data(), str.size(), neddle.data(), neddle.size());

                    if (result != n) {
                        printf("%s\n", ansi::seq("FAILED", ansi::RED).c_str());

                        printf("   string = '%s' (length %lu)\n", str.data(), str.size());
                        printf("   neddle = '%s' (length %lu)\n", neddle.data(), neddle.size());
                        printf("   expected result = %lu, actual result = %lu\n", n, result);

                        return false;
                    }
                }
            }
        }

        const auto msg = ansi::seq("OK", ansi::GREEN);
        printf("%s\n", msg.c_str());

        return true;
    }
};


int main() {

    UnitTests tests;
    int ret = EXIT_SUCCESS;

    puts("running unit tests");

    bool test_swar64     = !true;
    bool test_swar32     = !true;
#ifdef HAVE_SSE_INSTRUCTIONS
    bool test_sse41      = true;
    bool test_sse41unrl  = true;
    bool test_sse42      = true;
    bool test_sse_v2     = true;
    bool test_sse_naive  = true;
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
    bool test_avx2       = true;
    bool test_avx2_v2    = true;
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
    bool test_avx512f    = true;
    bool test_avx512f_v2 = true;
#endif
#ifdef HAVE_AVX512BW_INSTRUCTIONS
    bool test_avx512bw_v2 = true;
#endif
#ifdef HAVE_NEON_INSTRUCTIONS
    bool test_neon_v2    = true;
#endif
#ifdef HAVE_AARCH64_ARCHITECTURE
    bool test_aarch64_v2 = true;
#endif

    if (test_swar64) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return swar64_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("SWAR 64-bit (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

    if (test_swar32) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return swar32_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("SWAR 32-bit (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

#ifdef HAVE_SSE_INSTRUCTIONS
    if (test_sse_v2) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return sse2_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("SSE2 (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

    if (test_sse41) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return sse4_strstr(s1, n1, s2, n2);
        };

        if (!tests.run("SSE4.1", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

    if (test_sse41unrl) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return sse4_strstr_unrolled(s1, n1, s2, n2);
        };

        if (!tests.run("SSE4.1 (unrolled)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

    if (test_sse42) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return sse42_strstr(s1, n1, s2, n2);
        };

        if (!tests.run("SSE4.2", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

    if (test_sse_naive) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return sse_naive_strstr(s1, n1, s2, n2);
        };

        if (!tests.run("SSE naive", wrp)) {
            ret = EXIT_FAILURE;
        }
    }
#endif

#ifdef HAVE_AVX2_INSTRUCTIONS
    if (test_avx2) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return avx2_strstr(s1, n1, s2, n2);
        };

        if (!tests.run("AVX2", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

    if (test_avx2_v2) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return avx2_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("AVX2 (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }
#endif

#ifdef HAVE_AVX512F_INSTRUCTIONS
    if (test_avx512f) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return avx512f_strstr(s1, n1, s2, n2);
        };

        if (!tests.run("AVX512F", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

    if (test_avx512f_v2) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return avx512f_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("AVX512F (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }
#endif

#ifdef HAVE_AVX512BW_INSTRUCTIONS
    if (test_avx512bw_v2) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return avx512bw_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("AVX512BW (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }
#endif

#ifdef HAVE_NEON_INSTRUCTIONS
    if (test_neon_v2) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return neon_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("ARM Neon 32 bit (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }
#endif

#ifdef HAVE_AARCH64_ARCHITECTURE
    if (test_aarch64_v2) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return aarch64_strstr_v2(s1, n1, s2, n2);
        };

        if (!tests.run("AArch64 64 bit (v2)", wrp)) {
            ret = EXIT_FAILURE;
        }
    }
#endif

    return ret;
}

