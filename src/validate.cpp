#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

// ------------------------------------------------------------------------

#include "all_procedures.cpp"

// ------------------------------------------------------------------------

#include <utils/ansi.cpp>
#include "application_base.cpp"


class Application final: public ApplicationBase {

public:
    Application(const std::string& file_name, const std::string& words_name) {
        prepare(file_name, words_name);
    }

    bool run() {
        const auto n = words.size();

        auto db = all_procedures();

        for (size_t i = 0; i < n; i++) {

            if (i % 100 == 0) {
                print_progress(i, n);
            }

            const auto& word = words[i];
            const size_t reference = file.find(word);

            for (auto& item: db.procedures) {
                if (item.builtin) {
                    continue;
                }
                
                const size_t result = item.proc(file.data(), file.size(), word.data(), word.size());
                if (reference != result) {
                    putchar('\n');
                    const auto msg = ansi::seq("ERROR", ansi::RED);
                    printf("%s: std::find result = %lu, %s = %lu\n",
                        msg.data(), reference, item.name.c_str(), result);

                    printf("word: '%s' (length %lu)\n", word.data(), word.size());

                    return false;
                }
            }
        }

        print_progress(n, n);
        putchar('\n');

        const auto msg = ansi::seq("OK", ansi::GREEN);
        printf("%s\n", msg.c_str());

        return true;
    }


    static void print_help(const char* progname) {
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


    if (argc == 3) {
        try {
            Application app(argv[1], argv[2]);

            const auto ret = app.run();

            return ret ? EXIT_SUCCESS : EXIT_FAILURE;
        } catch (ApplicationBase::Error& err) {

            const auto msg = ansi::seq("Error: ", ansi::RED);
            printf("%s: %s\n", msg.data(), err.message.data());

            return EXIT_FAILURE;
        }
    } else {
        Application::print_help(argv[0]);

        return EXIT_FAILURE;
    }
}
