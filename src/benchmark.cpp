#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#include "all_procedures.cpp"

// ------------------------------------------------------------------------

#include <utils/ansi.cpp>
#include "benchmark.h"
#include "application_base.cpp"

class Application final: public ApplicationBase {

    Procedures db;

public:
    enum class TestType {
        OptimisticCase,
        Random,
        WorstCase
    };

    struct Parameters {
        size_t needle_position;
        size_t needle_size;
        size_t count;
        TestType test_type;
        std::string procedure_codes;
    };

public:
    Application(const Parameters& params)
        : db(all_procedures())
        , parameters(params) {

        prepare();
    }

    bool operator()() {

        // strstr is treated as built-in function by GCC
        // it seems it's wiped out in benchmark
        const bool measure_stdstring  = false;

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

        if (is_enabled('v')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse2_strstr_needle4(s, neddle);
            };

            measure(find, 'v');
        }

        if (is_enabled('w')) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse2_strstr_needle4_v2(s, neddle);
            };

            measure(find, 'w');
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

	if (is_enabled('u')) {

        auto find = [](const std::string& s, const std::string& neddle) -> size_t {

            return avx512bw_strstr_v3(s, neddle);
        };

        measure(find, 'u');
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
        std::printf("%s needle-position needle-size iteration-count test-name [procedures]\n", progname);
        std::puts("");
        std::puts("Parameters:");
        std::puts("");
        std::puts("  needle-position  position of the needle");
        std::puts("  needle-size      length of the needle");
        std::puts("  count            how many times test is repeated");
        std::puts("  test-name        one of 'optimistic', 'random', 'worst'");
        std::puts("  procedures       procedure code(s), listed below [by default all will be tested]");
        std::puts("");
        std::puts("Test kinds");
        std::puts("");
        std::puts("  optimistic       data before needle contains characters don't present in the needle");
        std::puts("  random           data before needle contains some random characters");
        std::puts("  worst            needle has form 'aaa...aaaXaaa...aaa', and data before is filled with the 'a'");
        std::puts("");
        std::puts("Following procedures are available:");
        for (auto& item: all_procedures().procedures) {
            printf(" [%c] %s\n", item.code, item.name.c_str());
        }
    }


private:
    volatile size_t sink;

    template <typename T_FIND>
    void measure(T_FIND find, char code) {

        BEST_TIME(/**/,
                  sink = find(input, needle),
                  db[code].name.c_str(),
                  parameters.count,
                  parameters.needle_position);
    }


    bool is_enabled(char proc) const {
        return (parameters.procedure_codes.empty())
            || (parameters.procedure_codes.find(proc) != std::string::npos);
    }

    void prepare_needle() {

        needle.append(parameters.needle_size/2, 'a');
        needle.append(1, 'X');
        needle.append(parameters.needle_size - needle.size(), 'a');
    }

    void prepare_input() {

        const size_t padding = 256;

        switch (parameters.test_type) {
            case TestType::OptimisticCase:
                input.assign(parameters.needle_position, '_');
                break;

            case TestType::WorstCase:
                input.assign(parameters.needle_position, 'a');
                break;

            case TestType::Random:
                for (size_t i=0; i < parameters.needle_position; i++) {
                    const char c = rand() % ('z' - 'a' + 1) + 'a';
                    input.push_back(c);
                }
                break;
        }

        input += needle;
        input.append(padding, '_'); // to make sure that memory after the needle is accessible
    }

    void prepare() {
        prepare_needle();
        prepare_input();
    }

    std::string needle;
    std::string input;
    Parameters parameters;
};


bool parse(int argc, char* argv[], Application::Parameters& p) {

    if (argc < 5) {
        return false;
    }

    for (int i=1; i < argc; i++) {
        const std::string tmp = argv[i];
        if (tmp == "-h" || tmp == "--help")
            return false;
    }

    p.needle_position = atoi(argv[1]);
    p.needle_size     = atoi(argv[2]);
    p.count           = atoi(argv[3]);

    if (p.needle_size < 3) {
        throw std::runtime_error("needle size must be greater than 2");
    }

    if (p.count == 0) {
        throw std::runtime_error("count must be greater than 0");
    }

    std::string tmp(argv[4]);
    if (tmp == "optimistic") {
        p.test_type = Application::TestType::OptimisticCase;
    } else if (tmp == "worst") {
        p.test_type = Application::TestType::WorstCase;
    } else if (tmp == "random") {
        p.test_type = Application::TestType::Random;
    } else {
        throw std::runtime_error("expected 'optimistic', 'worst' or 'random', got '" + tmp + "'");
    }

    if (argc >= 6) {
        p.procedure_codes = argv[5];
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

    } catch (std::runtime_error& err) {

        const auto msg = ansi::seq("Error", ansi::RED);
        printf("%s: %s\n", msg.data(), err.what());

        return EXIT_FAILURE;
    } catch (ApplicationBase::Error& err) {

        const auto msg = ansi::seq("Error", ansi::RED);
        printf("%s: %s\n", msg.data(), err.message.data());

        return EXIT_FAILURE;
    }
}

