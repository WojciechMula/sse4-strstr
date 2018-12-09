#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <chrono>

#include "all_procedures.cpp"

// ------------------------------------------------------------------------

#include <utils/ansi.cpp>
#include "application_base.cpp"

class Application final: public ApplicationBase {

    Procedures db;
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
        : db(all_procedures())
        , count(params.count)
        , procedure_codes(params.procedure_codes) {

        prepare(params.file_name, params.words_name);
    }

    bool operator()() {

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
        // On Raspberry Pi it's terribly slow, but on Aarch64
        // the 64-bit procedure is pretty fast
        const bool measure_swar64     = false;
#else
        const bool measure_swar64     = true;
#endif

        if (is_enabled('a')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return strstr_naive(s.data(), s.size(), neddle.data(), neddle.size());
            };

            measure(find, 'a');
        }

        if (is_enabled('b')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {
                const char* res = strstr(s.data(), neddle.data());

                if (res != nullptr) {
                    return res - s.data();
                } else {
                    return std::string::npos;
                }
            };

            measure(find, 'b');
        }

        if (measure_stdstring && is_enabled('c')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return s.find(neddle);
            };

            measure(find, 'c');
        }

        if (measure_swar64 && is_enabled('d')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return swar64_strstr_v2(s, neddle);
            };

            measure(find, 'd');
        }

        if (is_enabled('e')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return swar32_strstr_v2(s, neddle);
            };

            measure(find, 'e');
        }

#ifdef HAVE_SSE_INSTRUCTIONS
        if (is_enabled('f')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse2_strstr_v2(s, neddle);
            };

            measure(find, 'f');
        }

        if (is_enabled('g')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse4_strstr(s, neddle);
            };

            measure(find, 'g');
        }

        if (is_enabled('h')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse4_strstr_unrolled(s, neddle);
            };

            measure(find, 'h');
        }

        if (is_enabled('i')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse42_strstr(s, neddle);
            };

            measure(find, 'i');
        }

        if (is_enabled('j')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse_naive_strstr(s, neddle);
            };

            measure(find, 'j');
        }
#endif

#ifdef HAVE_AVX2_INSTRUCTIONS
        if (is_enabled('k')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_strstr(s, neddle);
            };

            measure(find, 'k');
        }

        if (is_enabled('l')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_strstr_v2(s, neddle);
            };

            measure(find, 'l');
        }

        if (is_enabled('m')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_naive_strstr(s, neddle);
            };

            measure(find, 'm');
        }

        if (is_enabled('n')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_naive_unrolled_strstr(s, neddle);
            };

            measure(find, 'n');
        }

        if (is_enabled('o')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_naive_strstr64(s, neddle);
            };

            measure(find, 'o');
        }
#endif

#ifdef HAVE_AVX512F_INSTRUCTIONS
        if (is_enabled('p')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx512f_strstr(s, neddle);
            };

            measure(find, 'p');
        }

        if (is_enabled('q')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx512f_strstr_v2(s, neddle);
            };

            measure(find, 'q');
        }
#endif

#ifdef HAVE_AVX512BW_INSTRUCTIONS
	if (is_enabled('r')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx512bw_strstr_v2(s, neddle);
            };

            measure(find, 'r');
        }
#endif

#ifdef HAVE_NEON_INSTRUCTIONS
        if (is_enabled('s')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return neon_strstr_v2(s, neddle);
            };

            measure(find, 's');
        }
#endif

#ifdef HAVE_AARCH64_ARCHITECTURE
        if (is_enabled('t')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return aarch64_strstr_v2(s, neddle);
            };

            measure(find, 't');
        }
#endif

        return true;
    }


    static void print_help(const char* progname) {
        std::printf("%s file words [count] [procedure]\n", progname);
        std::puts("");
        std::puts("Parameters:");
        std::puts("");
        std::puts("  file      - arbitrary file");
        std::puts("  words     - list of words in separate lines");
        std::puts("  count     - repeat count (optional, default = 10)");
        std::puts("  procedure - letter(s) from square brackets (by default all functions are checked)");
        std::puts("");
        std::puts("Following procedures ara available:");
        for (auto& item: all_procedures().procedures) {
            printf(" [%c] %s\n", item.code, item.name.c_str());
        }
    }


private:
    template <typename T_FIND>
    void measure(T_FIND find, char code) {

        printf("%-40s... ", db[code].name.c_str());
        fflush(stdout);

        size_t result = 0;

        const auto t1 = std::chrono::high_resolution_clock::now();

        auto k = count;
        while (k != 0) {
            for (const auto& word: words) {
                result += find(file, word);
            }

            k--;
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

    for (int i=1; i < argc; i++) {
        const std::string tmp = argv[i];
        if (tmp == "-h" || tmp == "--help")
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
