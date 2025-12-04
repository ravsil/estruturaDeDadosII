CXX := g++
CXXFLAGS := -std=c++11 -O2

.PHONY: all bmais run clean clearall

# Default: build the bmais executable
all: bmais

# Build from the files you provided
bmais: main.cpp bmais.cpp
	$(CXX) $(CXXFLAGS) main.cpp bmais.cpp -o bmais

# Run (Unix-like environments / msys/mingw)
run: bmais
	./bmais

clean:
	rm -f bmais bmais.exe
	rm -f metadata.bin index.bin data.bin

clearall: clean
	rm -f *.dat
	rm -rf particao*
