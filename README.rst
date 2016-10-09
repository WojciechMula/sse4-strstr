========================================================================
    SSE4 string search — modification of Karp-Rabin algorithm
========================================================================

Sample programs for articles:

* "SSE4 string search — modification of Karp-Rabin algorithm"
  (http://0x80.pl/articles/sse4_substring_locate.html)

* "SIMD-friendly Rabin-Karp modification"
  (http://0x80.pl/articles/simd-friendly-karp-rabin.html)

Subdirectory **original** contains 32-bit programs with inline assembly,
written in 2008 for the first article.  The **root directory** contains
new C++11 implementation using intrinsics, written in 2015. AVX2 and
AVX512 counterparts are also available.

The root directory contains also SSE2, AVX2 and AVX512 implementations
of a method described in the second article.


Usage
------------------------------------------------------------------------

Type ``make`` to build all programs.

Type ``make test``/``make test_avx2``/``make test_avx512`` to run unit
tests and validation tests.

Type ``make run``/``make run_avx2``/``make run_avx512`` to run performance
tests.


Performance results
------------------------------------------------------------------------

From subdirectory ``results``.

+--------------+-----------------------------------------------------------+
|              | CPU architecture                                          |
| procedure    +--------------+--------------+--------------+--------------+
| time [s]     | Westemere    | Haswell      | Skylake      | KNL          |
+==============+==============+==============+==============+==============+
| strstr       |  **0.82**    |    0.48      |    0.66      |    4.94      |
+--------------+--------------+--------------+--------------+--------------+
| string::find |   10.72      |    6.06      |    6.54      |   36.90      |
+--------------+--------------+--------------+--------------+--------------+
| SSE2 (v2)    |    1.29      |    0.93      |    0.70      |    8.68      |
+--------------+--------------+--------------+--------------+--------------+
| SSE4.1       |    1.40      |    0.82      |    0.75      |   19.32      |
+--------------+--------------+--------------+--------------+--------------+
| AVX2         |    --        |    0.57      |  **0.56**    |   13.15      |
+--------------+--------------+--------------+--------------+--------------+
| AVX2 (v2)    |    --        |  **0.43**    |  **0.46**    |  **4.80**    |
+--------------+--------------+--------------+--------------+--------------+
| AVX512F      |    --        |    --        |    --        |  **2.33**    |
+--------------+--------------+--------------+--------------+--------------+
| AVX512F (v2) |    --        |    --        |    --        |  **2.38**    |
+--------------+--------------+--------------+--------------+--------------+
