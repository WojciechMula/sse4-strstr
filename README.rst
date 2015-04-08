========================================================================
    SSE4 string search — modification of Karp-Rabin algorithm
========================================================================

Sample programs for article http://0x80.pl/articles/sse4_substring_locate.html

Subdirectory **original** contains original, 32-bit programs with
inline assembly, written in 2008.

The **root directory** contains new C++11 implementation using
intrinsics, written in 2015. The new code is about 2x slower
than ``std::strstr`` but faster than ``std::string::find`` about 7x
(measured on Core i5).


Usage
------------------------------------------------------------------------

Type ``make`` to build all programs.

Type ``make tests`` to run unit tests and validation tests.

Type ``make speedup`` to run performance tests.
