all: compile execute clear

generate: gerador_binario.cpp
	g++ gerador_binario.cpp -o gerador_binario
	./gerador_binario data.dat 100
	rm -f gerador_binario

compile:
	g++ naturalSelection.cpp -o ns
	g++ generator.cpp -o g
	g++ printer.cpp -o p

execute:
	./g data.dat 100
	./ns data.dat
	./p data.dat

reexecute:
	./ns data.dat
	./p data.dat

clear:
	rm -f ns g p

clearall:
	rm -f ns g p
	rm -f data.dat
	rm -rf particao*