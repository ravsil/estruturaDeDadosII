all: compile execute clean

compile:
	g++ main.cpp -o main

execute:
	./main test.dat

clean:
	rm -f main