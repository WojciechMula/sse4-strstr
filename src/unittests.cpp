#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#include <smmintrin.h>
#if defined(HAVE_AVX2_INSTRUCTIONS) || defined(HAVE_AVX512F_INSTRUCTIONS)
#   include <immintrin.h>
#endif

#include "common.h"
#include <utils/ansi.cpp>
#include <utils/sse.cpp>
#include <utils/bits.cpp>
#include "fixed-memcmp.cpp"
#include "sse4-strstr.cpp"
#include "sse4.2-strstr.cpp"
#include "sse2-strstr.cpp"
#ifdef HAVE_AVX2_INSTRUCTIONS
#   include <utils/avx2.cpp>
#   include "avx2-strstr.cpp"
#   include "avx2-strstr-v2.cpp"
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
#   include <utils/avx512.cpp>
#   include "avx512f-strstr.cpp"
#   include "avx512f-strstr-v2.cpp"
#endif

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

    bool test_sse41      = true;
    bool test_sse42      = true;
    bool test_sse_v2     = true;
#ifdef HAVE_AVX2_INSTRUCTIONS
    bool test_avx2       = true;
    bool test_avx2_v2    = true;
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
    bool test_avx512f    = true;
    bool test_avx512f_v2 = true;
#endif

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

    if (test_sse42) {
        auto wrp = [](const char* s1, size_t n1, const char* s2, size_t n2){
            return sse42_strstr(s1, n1, s2, n2);
        };

        if (!tests.run("SSE4.2", wrp)) {
            ret = EXIT_FAILURE;
        }
    }

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

    return ret;
}

