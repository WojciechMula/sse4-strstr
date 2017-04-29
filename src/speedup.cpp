#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <chrono>

#include "common.h"
#include <utils/bits.cpp>
#include "fixed-memcmp.cpp"
#include "swar64-strstr-v2.cpp"
#include "swar32-strstr-v2.cpp"
#ifdef HAVE_SSE_INSTRUCTIONS
#   include <utils/sse.cpp>
#   include "sse4-strstr.cpp"
#   include "sse4-strstr-unrolled.cpp"
#   include "sse4.2-strstr.cpp"
#   include "sse2-strstr.cpp"
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
#   include <utils/avx2.cpp>
#   include "avx2-strstr.cpp"
#   include "avx2-strstr-v2.cpp"
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
#   include "avx512f-strstr.cpp"
#   include "avx512f-strstr-v2.cpp"
#endif
#ifdef HAVE_NEON_INSTRUCTIONS
#   include <utils/neon.cpp>
#   include "neon-strstr-v2.cpp"
#endif
#ifdef HAVE_AARCH64_ARCHITECTURE
#   include "aarch64-strstr-v2.cpp"
#endif

// ------------------------------------------------------------------------

#include <utils/ansi.cpp>
#include "application_base.cpp"

class Application final: public ApplicationBase {

    std::size_t count;

public:
    Application() : count(1) {}

    bool operator()() {

        const bool measure_libc       = true;
#if defined(__GNUC__) && !defined(HAVE_NEON_INSTRUCTIONS)
        // GNU std::string::find was proven to be utterly slow,
        // don't waste our time on reconfirming that fact.
        //
        // (On Raspberry Pi it's fast, though)
        const bool measure_stdstring  = false;
#else
        const bool measure_stdstring  = true;
#endif
#if defined(HAVE_NEON_INSTRUCTIONS) && !defined(HAVE_AARCH64_ARCHITECTURE)
        // On Raspberry Pi it's terriby slow, but on Aarch64
        // the 64-bit procedure is pretty fast
        const bool measure_swar64     = false;
#else
        const bool measure_swar64     = true;
#endif

        const bool measure_swar32     = true;
#ifdef HAVE_SSE_INSTRUCTIONS
        const bool measure_sse2       = true;
        const bool measure_sse41      = true;
        const bool measure_sse41unrl  = true;
        const bool measure_sse42      = true;
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
        const bool measure_avx2       = true;
        const bool measure_avx2_v2    = true;
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
        const bool measure_avx512f    = true;
        const bool measure_avx512f_v2 = true;
#endif
#ifdef HAVE_NEON_INSTRUCTIONS
        const bool measure_neon_v2    = true;
#endif
#ifdef HAVE_AARCH64_ARCHITECTURE
        const bool measure_aarch64_v2 = true;
#endif

        if (measure_libc) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {
                const char* res = strstr(s.data(), neddle.data());

                if (res != nullptr) {
                    return res - s.data();
                } else {
                    return std::string::npos;
                }
            };

            printf("%-40s... ", "std::strstr");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_stdstring) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return s.find(neddle);
            };

            printf("%-40s... ", "std::string::find");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_swar64) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return swar64_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "SWAR 64-bit (generic)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_swar32) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return swar32_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "SWAR 32-bit (generic)");
            fflush(stdout);
            measure(find, count);
        }

#ifdef HAVE_SSE_INSTRUCTIONS
        if (measure_sse2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse2_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "SSE2 (generic)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_sse41) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse4_strstr(s, neddle);
            };

            printf("%-40s... ", "SSE4.1 (MPSADBW)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_sse41unrl) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse4_strstr_unrolled(s, neddle);
            };

            printf("%-40s... ", "SSE4.1 (MPSADBW unrolled)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_sse42) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse42_strstr(s, neddle);
            };

            printf("%-40s... ", "SSE4.2 (PCMPESTRM)");
            fflush(stdout);
            measure(find, count);
        }
#endif

#ifdef HAVE_AVX2_INSTRUCTIONS
        if (measure_avx2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_strstr(s, neddle);
            };

            printf("%-40s... ", "AVX2 (MPSADBW)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_avx2_v2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "AVX2 (generic)");
            fflush(stdout);
            measure(find, count);
        }
#endif

#ifdef HAVE_AVX512F_INSTRUCTIONS
        if (measure_avx512f) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx512f_strstr(s, neddle);
            };

            printf("%-40s... ", "AVX512F (MPSADBW-like)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_avx512f_v2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx512f_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "AVX512F (generic)");
            fflush(stdout);
            measure(find, count);
        }
#endif

#ifdef HAVE_NEON_INSTRUCTIONS
        if (measure_neon_v2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return neon_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "ARM Neon 32 bit (v2)");
            fflush(stdout);
            measure(find, count);
        }
#endif

#ifdef HAVE_AARCH64_ARCHITECTURE
        if (measure_aarch64_v2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return aarch64_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "AArch64 64 bit (v2)");
            fflush(stdout);
            measure(find, count);
        }
#endif

        return true;
    }


    void prepare(const std::string& file_name, const std::string& words_name, size_t cnt) {

        ApplicationBase::prepare(file_name, words_name);
        count = cnt;
    }


    void print_help(const char* progname) {
        std::printf("%s file words [count]\n", progname);
        std::puts("");
        std::puts(
            "Measure speed of following procedures: "
              "std::strstr"
            ", std::string::find"
            ", SWAR 64-bit (generic)"
            ", SWAR 32-bit (generic)"
#ifdef HAVE_SSE_INSTRUCTIONS
            ", SSE2 (generic)"
            ", SSE4.1 (MPSADBW)"
            ", SSE4.1 (MPSADBW unrolled)"
            ", SSE4.2 (PCMPESTRM)"
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
            ", AVX2 (MPSADBW)"
            ", AVX2 (generic)"
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
            ", AVX512F (MPSADBW-like)"
            ", AVX512F (generic)"
#endif
#ifdef HAVE_NEON_INSTRUCTIONS
            ", ARM Neon 32 bit (v2)"
#endif
#ifdef HAVE_AARCH64_ARCHITECTURE
            ", AArch64 64 bit (v2)"
#endif
        );
        std::puts("");
        std::puts("Parameters:");
        std::puts("");
        std::puts("  file  - arbitrary file");
        std::puts("  words - list of words in separate lines");
        std::puts("  count - repeat count (optional, default = 10)");
    }


private:
    template <typename T_FIND>
    void measure(T_FIND find, size_t count) {

        size_t result = 0;

        const auto t1 = std::chrono::high_resolution_clock::now();

        while (count != 0) {
            for (const auto& word: words) {
                result += find(file, word);
            }

            count--;
        }

        const auto t2 = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double> td = t2-t1;

        printf("reference result = %lu, time = %10.6f s\n", result, td.count());
    }
};


int main(int argc, char* argv[]) {

    Application app;

    if (argc == 3 || argc == 4) {
        try {
            size_t count = 10;
            if (argc == 4) {
                size_t tmp = atoi(argv[3]);
                if (tmp > 0) {
                    count = tmp;
                } else {
                    printf("repeat count '%s' invalid, keeping default %d\n", argv[3], count);
                }
            }
            app.prepare(argv[1], argv[2], count);

            return app() ? EXIT_SUCCESS : EXIT_FAILURE;

        } catch (ApplicationBase::Error& err) {

            const auto msg = ansi::seq("Error: ", ansi::RED);
            printf("%s: %s\n", msg.data(), err.message.data());

            return EXIT_FAILURE;
        }
    } else {
        app.print_help(argv[0]);

        return EXIT_FAILURE;
    }
}
