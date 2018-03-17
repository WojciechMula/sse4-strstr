#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <chrono>

#include "all.h"

// ------------------------------------------------------------------------

#include <utils/ansi.cpp>
#include "application_base.cpp"

class Application final: public ApplicationBase {

    std::size_t count;
    const std::string procedure_codes;

public:
    struct Parameters {
        std::string file_name;
        std::string words_name;
        size_t count = 10;
        std::string procedure_codes;
    };

public:
    Application(const Parameters& params)
        : count(params.count)
        , procedure_codes(params.procedure_codes) {
    
        prepare(params.file_name, params.words_name);
    }

    bool operator()() {

        const bool measure_scalar     = is_enabled('a');
        const bool measure_libc       = is_enabled('b');
#if defined(__GNUC__) && !defined(HAVE_NEON_INSTRUCTIONS)
        // GNU std::string::find was proven to be utterly slow,
        // don't waste our time on reconfirming that fact.
        //
        // (On Raspberry Pi it's fast, though)
        const bool measure_stdstring  = false;
#else
        const bool measure_stdstring  = is_enabled('c');
#endif
#if defined(HAVE_NEON_INSTRUCTIONS) && !defined(HAVE_AARCH64_ARCHITECTURE)
        // On Raspberry Pi it's terribly slow, but on Aarch64
        // the 64-bit procedure is pretty fast
        const bool measure_swar64     = false;
#else
        const bool measure_swar64     = is_enabled('d');
#endif

        const bool measure_swar32     = is_enabled('e');
#ifdef HAVE_SSE_INSTRUCTIONS
        const bool measure_sse2       = is_enabled('f');
        const bool measure_sse41      = is_enabled('g');
        const bool measure_sse41unrl  = is_enabled('h');
        const bool measure_sse42      = is_enabled('i');
        const bool measure_sse_naive  = is_enabled('j');
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
        const bool measure_avx2         = is_enabled('k');
        const bool measure_avx2_v2      = is_enabled('l');
        const bool measure_avx2_naive   = is_enabled('m');
        const bool measure_avx2_naive_unrolled   = is_enabled('n');
        const bool measure_avx2_naive64 = is_enabled('o');
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
        const bool measure_avx512f    = is_enabled('p');
        const bool measure_avx512f_v2 = is_enabled('q');
#endif
#ifdef HAVE_AVX512BW_INSTRUCTIONS
        const bool measure_avx512bw_v2 = is_enabled('r');
#endif
#ifdef HAVE_NEON_INSTRUCTIONS
        const bool measure_neon_v2 = is_enabled('s');
#endif
#ifdef HAVE_AARCH64_ARCHITECTURE
        const bool measure_aarch64_v2 = is_enabled('t');
#endif

        if (measure_scalar) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return strstr_naive(s.data(), s.size(), neddle.data(), neddle.size());
            };

            printf("%-40s... ", "naive scalar");
            fflush(stdout);
            measure(find, count);
        }

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

        if (measure_sse_naive) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse_naive_strstr(s, neddle);
            };

            printf("%-40s... ", "SSE (naive)");
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

        if (measure_avx2_naive) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_naive_strstr(s, neddle);
            };

            printf("%-40s... ", "AVX2 (naive)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_avx2_naive_unrolled) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_naive_unrolled_strstr(s, neddle);
            };

            printf("%-40s... ", "AVX2 (naive unrolled)");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_avx2_naive64) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_naive_strstr64(s, neddle);
            };

            printf("%-40s... ", "AVX2-wide (naive)");
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

#ifdef HAVE_AVX512BW_INSTRUCTIONS
	if (measure_avx512bw_v2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx512bw_strstr_v2(s, neddle);
            };

            printf("%-40s... ", "AVX512BW (generic)");
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


    static void print_help(const char* progname) {
        std::printf("%s file words [count] [procedure]\n", progname);
        std::puts("");
        std::puts(
            "Measure speed of following procedures: "
              "[a] scalar"
            ", [b] std::strstr"
            ", [c] std::string::find"
            ", [d] SWAR 64-bit (generic)"
            ", [e] SWAR 32-bit (generic)"
#ifdef HAVE_SSE_INSTRUCTIONS
            ", [f] SSE2 (generic)"
            ", [g] SSE4.1 (MPSADBW)"
            ", [h] SSE4.1 (MPSADBW unrolled)"
            ", [i] SSE4.2 (PCMPESTRM)"
            ", [j] SSE (naive)"
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
            ", [k] AVX2 (MPSADBW)"
            ", [l] AVX2 (generic)"
            ", [m] AVX2 (naive)"
            ", [n] AVX2 (naive unrolled)"
            ", [o] AVX2-wide (naive)"
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
            ", [p] AVX512F (MPSADBW-like)"
            ", [q] AVX512F (generic)"
#endif
#ifdef HAVE_AVX512BW_INSTRUCTIONS
            ", [r] AVX512BW (generic)"
#endif
#ifdef HAVE_NEON_INSTRUCTIONS
            ", [s] ARM Neon 32 bit (v2)"
#endif
#ifdef HAVE_AARCH64_ARCHITECTURE
            ", [t] AArch64 64 bit (v2)"
#endif
        );
        std::puts("");
        std::puts("Parameters:");
        std::puts("");
        std::puts("  file      - arbitrary file");
        std::puts("  words     - list of words in separate lines");
        std::puts("  count     - repeat count (optional, default = 10)");
        std::puts("  procedure - letter(s) from square brackets (by default all functions are checked)");
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


    bool is_enabled(char proc) const {
        return (procedure_codes.empty())
            || (procedure_codes.find(proc) != std::string::npos);
    }
};


bool parse(int argc, char* argv[], Application::Parameters& p) {
    if (argc < 3) {
        return false;
    }

    p.file_name = argv[1];
    p.words_name = argv[2];

    if (argc >= 4) {
        size_t tmp = atoi(argv[3]);
        if (tmp > 0) {
            p.count = tmp;
        } else {
            printf("repeat count '%s' invalid, keeping default %lu\n", argv[3], p.count);
        }
    }

    if (argc >= 5) {
        p.procedure_codes = argv[4];
    }

    return true;
}


int main(int argc, char* argv[]) {

    try {

        Application::Parameters params;
        if (!parse(argc, argv, params)) {
            Application::print_help(argv[0]);
            return EXIT_FAILURE;
        }

        Application app(params);
        return app() ? EXIT_SUCCESS : EXIT_FAILURE;

    } catch (ApplicationBase::Error& err) {

        const auto msg = ansi::seq("Error: ", ansi::RED);
        printf("%s: %s\n", msg.data(), err.message.data());

        return EXIT_FAILURE;
    }
}
