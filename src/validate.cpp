#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

// ------------------------------------------------------------------------

#include "common.h"
#include <utils/bits.cpp>
#include "fixed-memcmp.cpp"
#include "swar64-strstr-v2.cpp"
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

// ------------------------------------------------------------------------

#include <utils/ansi.cpp>
#include "application_base.cpp"


class Application final: public ApplicationBase {

public:
    bool run() {

        const auto n = words.size();

        for (size_t i = 0; i < n; i++) {

            const auto& word = words[i];

            const auto reference = file.find(word);
            const auto result_swar = swar64_strstr_v2(file, word);
#ifdef HAVE_SSE_INSTRUCTIONS
            const auto result_sse2 = sse2_strstr_v2(file, word);
            const auto result_sse41 = sse4_strstr(file, word);
            const auto result_sse41unrl = sse4_strstr_unrolled(file, word);
            const auto result_sse42 = sse42_strstr(file, word);
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
            const auto result_avx2    = avx2_strstr(file, word);
            const auto result_avx2_v2 = avx2_strstr_v2(file, word);
#endif
#ifdef HAVE_AVX512F_INSTRUCTIONS
            const auto result_avx512f = avx512f_strstr(file, word);
            const auto result_avx512f_v2 = avx512f_strstr_v2(file, word);
#endif
    
            if (i % 100 == 0) {
                print_progress(i, n);
            }

            if (reference != result_swar) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, swar= %lu\n",
                    msg.data(), reference, result_swar);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }

#ifdef HAVE_SSE_INSTRUCTIONS
            if (reference != result_sse2) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, sse2_string = %lu\n",
                    msg.data(), reference, result_sse2);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }

            if (reference != result_sse41) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, sse4_string = %lu\n",
                    msg.data(), reference, result_sse41);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }

            if (reference != result_sse41unrl) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, sse4_unrolled = %lu\n",
                    msg.data(), reference, result_sse41);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }

            if (reference != result_sse42) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, sse42_string = %lu\n",
                    msg.data(), reference, result_sse42);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }
#endif // HAVE_SSE_INSTRUCTIONS

#ifdef HAVE_AVX2_INSTRUCTIONS
            if (reference != result_avx2) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, avx2_string = %lu\n",
                    msg.data(), reference, result_avx2);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }

            if (reference != result_avx2_v2) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, avx2_string_v2 = %lu\n",
                    msg.data(), reference, result_avx2_v2);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }
#endif // HAVE_AVX2_INSTRUCTIONS

#ifdef HAVE_AVX512F_INSTRUCTIONS
            if (reference != result_avx512f) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, avx512f_string = %lu\n",
                    msg.data(), reference, result_avx512f);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }

            if (reference != result_avx512f_v2) {
                putchar('\n');
                const auto msg = ansi::seq("ERROR", ansi::RED);
                printf("%s: std::find result = %lu, avx512f_v2_string = %lu\n",
                    msg.data(), reference, result_avx512f_v2);

                printf("word: '%s' (length %lu)\n", word.data(), word.size());

                return false;
            }
#endif // HAVE_AVX512F_INSTRUCTIONS
        }

        print_progress(n, n);

        putchar('\n');

        const auto msg = ansi::seq("OK", ansi::GREEN);
        printf("%s\n", msg.c_str());

        return true;
    }


    void print_help(const char* progname) {
        std::printf("usage: %s [file] [words]\n", progname);
        std::puts("");
        std::puts("Search all words in a file using std::string::find and SSE4 procedure");
        std::puts("");
        std::puts("Parameters:");
        std::puts("");
        std::puts("  file  - arbitrary file");
        std::puts("  words - list of words in separate lines");
    }

private:
    void print_progress(size_t pos, size_t n) {

        printf("validating... %0.2f%% (%lu/%lu)\r", 100.0*pos/n, pos, n);
        fflush(stdout);
    }
};


int main(int argc, char* argv[]) {

    Application app;

    if (argc == 3) {
        try {
            app.prepare(argv[1], argv[2]);

            const auto ret = app.run();

            return ret ? EXIT_SUCCESS : EXIT_FAILURE;
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
