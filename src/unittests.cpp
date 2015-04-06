#include <cstdio>
#include <cstdint>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#include <smmintrin.h>

#include <utils/ansi.cpp>
#include <utils/sse.cpp>
#include <utils/bits.cpp>
#include "sse4-strstr.cpp"


class UnitTests final {

public:
    bool operator()() {

        std::printf("running unit tests... ");
        std::fflush(stdout);

        for (size_t size = 1; size < 64; size++) {

            const std::string neddle = "$" + std::string(size, 'x') + "#";

            for (size_t n = 0; n < 3*16; n++) {

                const std::string prefix(n, '.');

                for (size_t k = 0; k < 3*16; k++) {
                    // '.' * k + '$' + 'x' * size + '#' + '.' * k
                    
                    const std::string suffix(k, '.');
                    const std::string str = prefix + neddle + suffix;
                    
                    const auto result = sse4_strstr(str.data(), str.size(), neddle.data(), neddle.size());

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

    return tests() ? EXIT_SUCCESS : EXIT_FAILURE;
}

