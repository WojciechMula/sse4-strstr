========================================================================
    SSE4 string search — modification of Karp-Rabin algorithm
========================================================================

Sample programs for article http://0x80.pl/articles/sse4_substring_locate.html

Subdirectory **original** contains original, 32-bit programs with
inline assembly, written in 2008.

The **root directory** contains new C++11 implementation using
intrinsics, written in 2015.  AVX2 counterparts are also available.


Usage
------------------------------------------------------------------------

Type ``make`` to build all programs.

Type ``make test``/``make test_avx2`` to run unit tests and validation tests.

Type ``make run``/``make run_avx2`` to run performance tests.


Performance results
------------------------------------------------------------------------

From subdirectory ``results``.

+--------------+-----------------------------------------------------------+
|              | procedure time in seconds                                 |
|              +--------------+--------------+--------------+--------------+
| architecture | strstr       | string::find | SSE4.1       | AVX2         |
+==============+==============+==============+==============+==============+
| Westemere    | 0.83         | 10.71        | 1.40         | N/A          |
+--------------+--------------+--------------+--------------+--------------+
| Haswell      | 0.48         |  6.06        | 0.82         | 0.57         |
+--------------+--------------+--------------+--------------+--------------+
| Skylake      | 0.66         |  6.54        | 0.64         | 0.46         |
+--------------+--------------+--------------+--------------+--------------+
