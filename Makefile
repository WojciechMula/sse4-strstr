.PHONY: all clean

FLAGS=-std=c++11 -O3 -Wall -Wextra -pedantic -I.
FLAGS_SSE4=$(FLAGS) -msse4.1 
FLAGS_AVX2=$(FLAGS) -mavx2 -DHAVE_AVX2_INSTRUCTIONS
FLAGS_AVX512=$(FLAGS) -mavx512f -DHAVE_AVX2_INSTRUCTIONS -DHAVE_AVX512F_INSTRUCTIONS

DEPS=utils/ansi.cpp utils/bits.cpp common.h
DEPS_SSE4=sse4-strstr.cpp sse2-strstr.cpp utils/sse.cpp $(DEPS)
DEPS_AVX2=avx2-strstr.cpp utils/avx2.cpp $(DEPS_SSE4)
DEPS_AVX512=avx512f-strstr.cpp avx512f-strstr-v2.cpp utils/avx512.cpp $(DESP_AVX2)

ALL=validate \
    speedup \
    unittests \
    validate_avx2 \
    speedup_avx2 \
    unittests_avx2 \
    validate_avx512 \
    speedup_avx512 \
    unittests_avx512

all: $(ALL)

validate: src/validate.cpp src/application_base.cpp $(DEPS_SSE4)
	$(CXX) $(FLAGS_SSE4) src/validate.cpp -o $@

speedup: src/speedup.cpp src/application_base.cpp $(DEPS_SSE4)
	$(CXX) $(FLAGS_SSE4) -DNDEBUG src/speedup.cpp -o $@

unittests: src/unittests.cpp $(DEPS_SSE4)
	$(CXX) $(FLAGS_SSE4) src/unittests.cpp -o $@

validate_avx2: src/validate.cpp src/application_base.cpp $(DEPS_AVX2)
	$(CXX) $(FLAGS_AVX2) src/validate.cpp -o $@

speedup_avx2: src/speedup.cpp src/application_base.cpp $(DEPS_AVX2)
	$(CXX) $(FLAGS_AVX2) -DNDEBUG  src/speedup.cpp -o $@

unittests_avx2: src/unittests.cpp $(DEPS_AVX2)
	$(CXX) $(FLAGS_AVX2) src/unittests.cpp -o $@

validate_avx512: src/validate.cpp src/application_base.cpp $(DEPS_AVX512)
	$(CXX) $(FLAGS_AVX512) src/validate.cpp -o $@

speedup_avx512: src/speedup.cpp src/application_base.cpp $(DEPS_AVX512)
	$(CXX) $(FLAGS_AVX512) -DNDEBUG  src/speedup.cpp -o $@

unittests_avx512: src/unittests.cpp $(DEPS_AVX512)
	$(CXX) $(FLAGS_AVX512) src/unittests.cpp -o $@

data/i386.txt:
	wget http://css.csail.mit.edu/6.858/2013/readings/i386.txt
	mv i386.txt data/i386.txt

data/words: data/i386.txt
	sh make_words.sh $^ $@

test: unittests validate data/words data/i386.txt
	./unittests
	./validate data/i386.txt data/words

run: speedup data/words data/i386.txt
	./speedup data/i386.txt data/words 

test_avx2: unittests_avx2 validate_avx2 data/words data/i386.txt
	./unittests_avx2
	./validate_avx2 data/i386.txt data/words

run_avx2: speedup_avx2 data/words data/i386.txt
	./speedup_avx2 data/i386.txt data/words 

test_avx512: unittests_avx512 validate_avx512 data/words data/i386.txt
	./unittests_avx512
	./validate_avx512 data/i386.txt data/words

run_avx512: speedup_avx512 data/words data/i386.txt
	./speedup_avx512 data/i386.txt data/words 

clean:
	rm -f $(ALL)
