================================================================================
             SIMD-friendly algorithms for substring searching
================================================================================

Sample programs for article "SIMD-friendly algorithms for substring searching"
(http://0x80.pl/articles/simd-strfind.html).

The **root directory** contains C++11 procedures implemented using intrinsics
for SSE, SSE4, AVX2, AVX512F, AVX512BW and ARM Neon (both ARMv7 and ARMv8).

The subdirectory **original** contains 32-bit programs with inline assembly,
written in 2008 for another article__.

__ http://0x80.pl/articles/sse4_substring_locate.html


Usage
------------------------------------------------------------------------

To run unit and validation tests type ``make test_ARCH``, to run
performance tests type ``make run_ARCH``. Value ``ARCH`` selectes
the CPU architecture:

* sse4,
* avx2,
* avx512f,
* avx512bw,
* arm,
* aarch64.


Performance results
------------------------------------------------------------------------

The subdirectory ``results`` contains raw timings from various computers.
