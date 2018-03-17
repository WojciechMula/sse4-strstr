/*
    The following templates implement the loop, where K is a template parameter.

        for (unsigned i=1; i < K; i++) {
            const __m256i substring = _mm256_alignr_epi8(next1, curr, i);
            eq = _mm256_and_si256(eq, _mm256_cmpeq_epi8(substring, broadcasted[i]));
        }

    Clang complains that the loop parameter `i` is a variable and it cannot be
    applied as a parameter _mm256_alignr_epi8.  GCC somehow deals with it.
*/

#ifdef __clang__

template <size_t K, int i, bool terminate>
struct inner_loop_aux;

template <size_t K, int i>
struct inner_loop_aux<K, i, false> {
    void operator()(__m256i& eq, const __m256i& next1, const __m256i& curr, const __m256i (&broadcasted)[K]) {
        const __m256i substring = _mm256_alignr_epi8(next1, curr, i);
        eq = _mm256_and_si256(eq, _mm256_cmpeq_epi8(substring, broadcasted[i]));
        inner_loop_aux<K, i + 1, i + 1 == K>()(eq, next1, curr, broadcasted);
    }
};

template <size_t K, int i>
struct inner_loop_aux<K, i, true> {
    void operator()(__m256i&, const __m256i&, const __m256i&, const __m256i (&)[K]) {
        // nop
    }
};

template <size_t K>
struct inner_loop {
    void operator()(__m256i& eq, const __m256i& next1, const __m256i& curr, const __m256i (&broadcasted)[K]) {
        static_assert(K > 0, "wrong value");
        inner_loop_aux<K, 0, false>()(eq, next1, curr, broadcasted);
    }
};

#endif
