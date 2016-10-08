#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>
#include <chrono>

#include <smmintrin.h>
#ifdef HAVE_AVX2_INSTRUCTIONS
#   include <immintrin.h>
#endif

#include <utils/sse.cpp>
#include <utils/bits.cpp>
#include "sse4-strstr.cpp"
#ifdef HAVE_AVX2_INSTRUCTIONS
#   include <utils/avx2.cpp>
#   include "avx2-strstr.cpp"
#endif

// ------------------------------------------------------------------------

#include <utils/ansi.cpp>
#include "application_base.cpp"

class Application final: public ApplicationBase {
    
    std::size_t count;

public:
    Application() : count(10) {
    }

    bool operator()() {

        const bool measure_libc      = true;
        const bool measure_stdstring = true;
        const bool measure_sse4      = true;
#ifdef HAVE_AVX2_INSTRUCTIONS
        const bool measure_avx2      = true;
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

            printf("%-20s... ", "std::strstr");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_stdstring) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return s.find(neddle);
            };

            printf("%-20s... ", "std::string::find");
            fflush(stdout);
            measure(find, count);
        }

        if (measure_sse4) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return sse4_strstr(s, neddle);
            };

            printf("%-20s... ", "SSE4");
            fflush(stdout);
            measure(find, count);
        }

#ifdef HAVE_AVX2_INSTRUCTIONS
        if (measure_avx2) {

            auto find = [](const std::string& s, const std::string& neddle) -> size_t {

                return avx2_strstr(s, neddle);
            };

            printf("%-20s... ", "AVX2");
            fflush(stdout);
            measure(find, count);
        }
#endif

        return true;
    }

    
    void print_help(const char* progname) {
        std::printf("%s file words\n", progname);
        std::puts("");
        std::puts(
            "Measure speed of following procedures: "
              "std::strstr"
            ", std::string::find"
            ", SSE4"
#ifdef HAVE_AVX2_INSTRUCTIONS
            ", AVX2"
#endif
        );
        std::puts("");
        std::puts("Parameters:");
        std::puts("");
        std::puts("  file  - arbitrary file");
        std::puts("  words - list of words in separate lines");
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

    if (argc == 3) {
        try {
            app.prepare(argv[1], argv[2]);

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
