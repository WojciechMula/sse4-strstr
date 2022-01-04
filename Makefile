.PHONY: all clean compile_intel

FLAGS=-std=c++11 -O3 -Wall -Wextra -pedantic -I. $(CXXFLAGS)
FLAGS_INTEL=$(FLAGS) -DHAVE_SSE_INSTRUCTIONS
FLAGS_SSE4=$(FLAGS_INTEL) -msse4.2
FLAGS_AVX2=$(FLAGS_INTEL) -mavx2 -DHAVE_AVX2_INSTRUCTIONS
FLAGS_AVX512F=$(FLAGS_INTEL) -mavx512f -DHAVE_AVX2_INSTRUCTIONS -DHAVE_AVX512F_INSTRUCTIONS
FLAGS_AVX512BW=$(FLAGS_INTEL) -mavx512bw -DHAVE_AVX2_INSTRUCTIONS -DHAVE_AVX512F_INSTRUCTIONS -DHAVE_AVX512BW_INSTRUCTIONS
FLAGS_ARM=$(FLAGS) -mfpu=neon -DHAVE_NEON_INSTRUCTIONS
FLAGS_AARCH64=$(FLAGS) -DHAVE_NEON_INSTRUCTIONS -DHAVE_AARCH64_ARCHITECTURE

DEPS=utils/ansi.cpp utils/bits.cpp common.h fixed-memcmp.cpp
DEPS_SCALAR=swar64-strstr-v2.cpp swar32-strstr-v2.cpp scalar.cpp
DEPS_SSE4=sse4-strstr.cpp sse4-strstr-unrolled.cpp sse4.2-strstr.cpp sse2-strstr.cpp sse-naive-strstr.cpp sse2-needle4.cpp utils/sse.cpp $(DEPS) $(DEPS_SCALAR)
DEPS_AVX2=avx2-*.cpp utils/avx2.cpp $(DEPS_SSE4)
DEPS_AVX512F=avx512f-*.cpp utils/avx512.cpp $(DEPS_AVX2)
DEPS_AVX512BW=avx512bw-*.cpp utils/avx512.cpp $(DEPS_AVX512F)
DEPS_ARM=neon-strstr-v2.cpp $(DEPS) $(DEPS_SCALAR)
DEPS_AARCH64=aarch64-strstr-v2.cpp $(DEPS_ARM)

ALL_INTEL=\
    validate_sse4 \
    speedup_sse4 \
    benchmark_sse4 \
    unittests_sse4 \
    validate_avx2 \
    speedup_avx2 \
    benchmark_avx2 \
    unittests_avx2 \
    validate_avx512f \
    speedup_avx512f \
    benchmark_avx512f \
    unittests_avx512f \
    speedup_avx512bw \
    benchmark_avx512bw \
    validate_avx512bw \
    unittests_avx512bw \

ALL_ARM=\
    validate_arm \
    unittests_arm \
    speedup_arm

ALL_AARCH64=\
    validate_aarch64 \
    unittests_aarch64 \
    speedup_aarch64

ALL=$(ALL_INTEL) $(ALL_ARM) $(ALL_AARCH64)

all:
	@echo "select target test_ARCH or run_ARCH"
	@echo
	@echo "test_ARCH runs unit and validation tests"
	@echo "run_ARCH  runs performance tests"
	@echo
	@echo "ARCH might be:"
	@echo "* sse4"
	@echo "* avx2"
	@echo "* avx512f"
	@echo "* avx512bw"
	@echo "* arm"
	@echo "* aarch64"

build_intel:   $(ALL_INTEL)
build_arm:     $(ALL_ARM)
build_aarch64: $(ALL_AARCH64)

UNITTESTS_DEPS=src/unittests.cpp src/all_procedures.cpp
VALIDATE_DEPS=src/validate.cpp src/application_base.cpp src/all_procedures.cpp
SPEEDUP_DEPS=src/speedup.cpp src/application_base.cpp src/all_procedures.cpp
BENCHMARK_DEPS=src/benchmark.cpp src/benchmark.h src/application_base.cpp src/all_procedures.cpp

validate_sse4: $(VALIDATE_DEPS) $(DEPS_SSE4)
	$(CXX) $(FLAGS_SSE4) src/validate.cpp -o $@

speedup_sse4: $(SPEEDUP_DEPS) $(DEPS_SSE4)
	$(CXX) $(FLAGS_SSE4) -DNDEBUG src/speedup.cpp -o $@

benchmark_sse4: $(BENCHMARK_DEPS) $(DEPS_SSE4)
	$(CXX) $(FLAGS_SSE4) -DNDEBUG src/benchmark.cpp -o $@

unittests_sse4: $(UNITTESTS_DEPS) $(DEPS_SSE4)
	$(CXX) $(FLAGS_SSE4) src/unittests.cpp -o $@

validate_avx2: $(VALIDATE_DEPS) $(DEPS_AVX2)
	$(CXX) $(FLAGS_AVX2) src/validate.cpp -o $@

speedup_avx2: $(SPEEDUP_DEPS) $(DEPS_AVX2)
	$(CXX) $(FLAGS_AVX2) -DNDEBUG  src/speedup.cpp -o $@

benchmark_avx2: $(BENCHMARK_DEPS) $(DEPS_SSE4)
	$(CXX) $(FLAGS_AVX2) -DNDEBUG src/benchmark.cpp -o $@

unittests_avx2: $(UNITTESTS_DEPS) $(DEPS_AVX2)
	$(CXX) $(FLAGS_AVX2) src/unittests.cpp -o $@

validate_avx512f: $(VALIDATE_DEPS) $(DEPS_AVX512F)
	$(CXX) $(FLAGS_AVX512F) src/validate.cpp -o $@

benchmark_avx512f: $(BENCHMARK_DEPS) $(DEPS_SSE4)
	$(CXX) $(FLAGS_AVX512F) -DNDEBUG src/benchmark.cpp -o $@

speedup_avx512f: $(SPEEDUP_DEPS) $(DEPS_AVX512F)
	$(CXX) $(FLAGS_AVX512F) -DNDEBUG  src/speedup.cpp -o $@

unittests_avx512f: $(UNITTESTS_DEPS) $(DEPS_AVX512F)
	$(CXX) $(FLAGS_AVX512F) src/unittests.cpp -o $@

validate_avx512bw: $(VALIDATE_DEPS) $(DEPS_AVX512BW)
	$(CXX) $(FLAGS_AVX512BW) src/validate.cpp -o $@

speedup_avx512bw: $(SPEEDUP_DEPS) $(DEPS_AVX512BW)
	$(CXX) $(FLAGS_AVX512BW) -DNDEBUG src/speedup.cpp -o $@

benchmark_avx512bw: $(BENCHMARK_DEPS) $(DEPS_SSE4)
	$(CXX) $(FLAGS_AVX512BW) -DNDEBUG src/benchmark.cpp -o $@

unittests_avx512bw: $(UNITTESTS_DEPS) $(DEPS_AVX512BW)
	$(CXX) $(FLAGS_AVX512BW) src/unittests.cpp -o $@

validate_arm: $(VALIDATE_DEPS) $(DEPS_ARM)
	$(CXX) $(FLAGS_ARM) src/validate.cpp -o $@

speedup_arm: $(SPEEDUP_DEPS) $(DEPS_ARM)
	$(CXX) $(FLAGS_ARM) -DNDEBUG  src/speedup.cpp -o $@

unittests_arm: $(UNITTESTS_DEPS) $(DEPS_ARM)
	$(CXX) $(FLAGS_ARM) src/unittests.cpp -o $@

validate_aarch64: $(VALIDATE_DEPS) $(DEPS_AARCH64)
	$(CXX) $(FLAGS_AARCH64) src/validate.cpp -o $@

speedup_aarch64: $(SPEEDUP_DEPS) $(DEPS_AARCH64)
	$(CXX) $(FLAGS_AARCH64) -DNDEBUG  src/speedup.cpp -o $@

unittests_aarch64: $(UNITTESTS_DEPS) $(DEPS_ARM)
	$(CXX) $(FLAGS_AARCH64) src/unittests.cpp -o $@

data/i386.txt:
	wget http://css.csail.mit.edu/6.858/2013/readings/i386.txt
	mv i386.txt data/i386.txt

data/words: data/i386.txt
	sh make_words.sh $^ $@

test_sse4: unittests_sse4 validate_sse4 data/words data/i386.txt
	./unittests_sse4
	./validate_sse4 data/i386.txt data/words

run_sse4: speedup_sse4 data/words data/i386.txt
	./speedup_sse4 data/i386.txt data/words

test_avx2: unittests_avx2 validate_avx2 data/words data/i386.txt
	./unittests_avx2
	./validate_avx2 data/i386.txt data/words

run_avx2: speedup_avx2 data/words data/i386.txt
	./speedup_avx2 data/i386.txt data/words

test_avx512f: unittests_avx512f validate_avx512f data/words data/i386.txt
	./unittests_avx512f
	./validate_avx512f data/i386.txt data/words

run_avx512f: speedup_avx512f data/words data/i386.txt
	./speedup_avx512f data/i386.txt data/words

run_avx512bw: speedup_avx512bw data/words data/i386.txt
	./speedup_avx512bw data/i386.txt data/words

test_avx512bw: unittests_avx512bw validate_avx512bw data/words data/i386.txt
	./unittests_avx512bw
	./validate_avx512bw data/i386.txt data/words

test_arm: unittests_arm validate_arm data/words data/i386.txt
	./unittests_arm
	./validate_arm data/i386.txt data/words

run_arm: speedup_arm data/words data/i386.txt
	# my Raspberry Pi is slow, repeat count = 1 is enough
	./$< data/i386.txt data/words 1

test_aarch64: unittests_aarch64 validate_aarch64 data/words data/i386.txt
	./unittests_aarch64
	./validate_aarch64 data/i386.txt data/words

run_aarch64: speedup_aarch64 data/words data/i386.txt
	./$< data/i386.txt data/words 1

compile_intel: $(ALL_INTEL)

clean:
	rm -f $(ALL)
