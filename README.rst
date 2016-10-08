========================================================================
    SSE4 string search — modification of Karp-Rabin algorithm
========================================================================

Sample programs for article http://0x80.pl/articles/sse4_substring_locate.html

Subdirectory **original** contains original, 32-bit programs with
inline assembly, written in 2008.

The **root directory** contains new C++11 implementation using
intrinsics, written in 2015. AVX2 and AVX512 counterparts are also
available.


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

+--------------+-----------------------------------------------------------------------------------------+
|              | procedure time in seconds                                                               |
|              +--------------+--------------+--------------+--------------+--------------+--------------+
| architecture | strstr       | string::find | SSE4.1       | AVX2         | AVX512       | AVX512 (v2)  |
+==============+==============+==============+==============+==============+==============+==============+
| Westemere    | 0.83         | 10.71        | 1.40         | N/A          | N/A          | N/A          |
+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
| Haswell      | 0.48         |  6.06        | 0.82         | 0.57         | N/A          | N/A          |
+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
| Skylake      | 0.66         |  6.54        | 0.64         | 0.46         | N/A          | N/A          |
+--------------+--------------+--------------+--------------+--------------+--------------+--------------+
| KNL          | 4.96         | 36.94        | 19.35        | 13.17        | 2.34         | 2.39         |
+--------------+--------------+--------------+--------------+--------------+--------------+--------------+

