========================================================================
    SIMD-friendly algorithms for substring searching
========================================================================

Sample programs for article "SIMD-friendly algorithms for substring searching"
(http://0x80.pl/articles/simd-strfind.html).

The **root directory** contains C++11 procedures implemented using intrinsics
for SSE, SSE4, AVX2 and AVX512F.

The subdirectory **original** contains 32-bit programs with inline assembly,
written in 2008 for another article__.

__ http://0x80.pl/articles/sse4_substring_locate.html


Usage
------------------------------------------------------------------------

Type ``make`` to build all programs.

Type ``make test``/``make test_avx2``/``make test_avx512`` to run unit
tests and validation tests.

Type ``make run``/``make run_avx2``/``make run_avx512`` to run performance
tests.


Performance results
------------------------------------------------------------------------

The subdirectory ``results`` contains raw timings from various computers.
