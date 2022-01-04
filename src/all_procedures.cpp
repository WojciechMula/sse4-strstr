#include "all.h"

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

using str_find_fun = size_t (*)(const char*, size_t, const char*, size_t);

struct Procedures {

    struct Item {
        str_find_fun proc;
        std::string name;
        char code;
        bool builtin;

        Item(str_find_fun proc_, const char* name_, char code_, bool builtin_ = false)
            : proc(proc_)
            , name(name_)
            , code(code_)
            , builtin(builtin_) {}
    };

    std::vector<Item> procedures;

    const Item& operator[](char code) {
        auto pred = [code](const Item& item){return item.code == code;};
        auto it = std::find_if(procedures.begin(), procedures.end(), pred);

        if (it == procedures.end()) {
            throw std::logic_error("can't find procedure with code '" + std::string(1, code) + "'");
        }

        return *it;
    }
};

size_t strstr_libc(const char* s, size_t, const char* needle, size_t) {
    const char* ptr = strstr(s, needle);
    if (ptr) {
        return ptr - s;
    } else {
        return std::string::npos;
    }
}

Procedures all_procedures() {

    Procedures db;

    db.procedures.emplace_back(
        strstr_naive,
        "scalar (naive)",
        'a'
    );

    db.procedures.emplace_back(
        strstr_libc,
        "std::strstr",
        'b',
        true
    );

    db.procedures.emplace_back(
        nullptr,
        "std::string::find",
        'c',
        true
    );

#define REGISTER(code, name, procedure)                 \
    {                                                   \
        str_find_fun f = procedure;                     \
        db.procedures.emplace_back(f, name, code);      \
    }

    REGISTER('d', "SWAR 64-bit (generic)", swar64_strstr_v2);
    REGISTER('e', "SWAR 32-bit (generic)", swar32_strstr_v2);

#ifdef HAVE_SSE_INSTRUCTIONS
    REGISTER('f', "SSE2 (generic)", sse2_strstr_v2);
    REGISTER('g', "SSE4.1 (MPSADBW)", sse4_strstr);
    REGISTER('h', "SSE4.1 (MPSADBW unrolled)", sse4_strstr_unrolled);
    REGISTER('i', "SSE4.2 (PCMPESTRM)", sse42_strstr);
    REGISTER('j', "SSE (naive)", sse_naive_strstr);
    REGISTER('v', "SSE2 (4-byte needle)", sse2_strstr_needle4);
    REGISTER('w', "SSE2 (4-byte needle v2)", sse2_strstr_needle4_v2);
#endif
#ifdef HAVE_AVX2_INSTRUCTIONS
    REGISTER('k', "AVX2 (MPSADBW)", avx2_strstr);
    REGISTER('l', "AVX2 (generic)", avx2_strstr_v2);
    REGISTER('m', "AVX2 (naive)", avx2_naive_strstr);
    REGISTER('n', "AVX2 (naive unrolled)", avx2_naive_unrolled_strstr);
    REGISTER('o', "AVX2-wide (naive)", avx2_naive_strstr64);
#endif

#ifdef HAVE_AVX512F_INSTRUCTIONS
    REGISTER('p', "AVX512F (MPSADBW-like)", avx512f_strstr);
    REGISTER('q', "AVX512F (generic)", avx512f_strstr_v2);
#endif

#ifdef HAVE_AVX512BW_INSTRUCTIONS
    REGISTER('r', "AVX512BW (generic)", avx512bw_strstr_v2);
    REGISTER('s', "AVX512BW (masked)", avx512bw_strstr_v3);
#endif

#ifdef HAVE_NEON_INSTRUCTIONS
    REGISTER('t', "ARM Neon 32 bit (v2)", neon_strstr_v2);
#endif

#ifdef HAVE_AARCH64_ARCHITECTURE
    REGISTER('u', "AArch64 64 bit (v2)", aarch64_strstr_v2);
#endif

#undef REGISTER
    return db;
}
