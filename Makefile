all: compile execute clear

compile:
        g++ -std=c++11 -Wall -O2 -c HashTable.cpp -o h.o
        g++ -std=c++11 -Wall -O2 -c experimento.cpp -o ex.o
        g++ -std=c++11 -Wall -O2 h.o ex.o -o ex

execute:
        ./ex

clear:
        rm -f ex *.o
