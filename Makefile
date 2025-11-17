CXX = g++

CXXFLAGS = -std=c++11 -Wall -O2

all: compile execute

compile_orig: generator printer naturalSelection

compile_hash: HashTable.o experimento.o
	$(CXX) $(CXXFLAGS) HashTable.o experimento.o -o experimento

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

compile: compile_orig compile_hash

execute:
	./g data.dat 100
	./ns data.dat
	./p data.dat

run_hash: compile_hash
	./experimento

clear:
	rm -f ns g p experimento *.o # Adicionado 'experimento' e '*.o'

clearall:
	rm -f ns g p experimento *.o # Adicionado 'experimento' e '*.o'
	rm -f data.dat
	rm -rf particao*