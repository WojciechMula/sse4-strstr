.PHONY: all

CC=g++
FLAGS=-std=c++11 -msse4.1 -O3 -Wall -Wextra -pedantic -I.

DEPS=sse4-strstr.cpp utils/*.cpp

ALL=validate speedup unittests

all: $(ALL)

validate: src/validate.cpp src/application_base.cpp $(DEPS)
	$(CC) $(FLAGS) src/validate.cpp -o validate

speedup: src/speedup.cpp src/application_base.cpp $(DEPS)
	$(CC) $(FLAGS) src/speedup.cpp -o speedup

unittests: src/unittests.cpp $(DEPS)
	$(CC) $(FLAGS) src/unittests.cpp -o unittests

data/i386.txt:
	wget http://css.csail.mit.edu/6.858/2013/readings/i386.txt data/i386.txt

data/words: data/i386.txt
	sh make_words.sh $^ $@

test: unittests validate data/words data/i386.txt
	./unittests
	./validate data/i386.txt data/words

run: speedup data/words data/i386.txt
	./speedup data/i386.txt data/words 

clean:
	rm -f $(ALL)
