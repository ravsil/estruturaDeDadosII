CXX := g++
CXXFLAGS := -std=c++11 -O2

.PHONY: all bmais test run run-test clean clearall

all: run run-test

bmais: main.cpp bmais.cpp
	$(CXX) $(CXXFLAGS) main.cpp bmais.cpp -o bmais

run: bmais
	./bmais

test: test.cpp bmais.cpp
	$(CXX) $(CXXFLAGS) test.cpp bmais.cpp -o test

run-test: test
	./test

clean:
	rm -f bmais test
	rm -f metadata.bin index.bin data.bin

clearall: clean
	rm -f *.dat
	rm -rf particao*
