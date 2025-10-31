all: compile execute clear

generate: gerador_binario.cpp
	g++ gerador_binario.cpp -o gerador_binario
	./gerador_binario data.dat 100
	rm -f gerador_binario

compile:
	g++ naturalSelection.cpp -o ns
	g++ generator.cpp -o g
	g++ printer.cpp -o p
	g++ intercalacao.cpp -o m

execute:
	./g data.dat 100
	./ns data.dat
	./m data_sorted.dat p*.dat
	./p data_sorted.dat

reexecute:
	./ns data.dat
	./p data.dat

clear:
	rm -f ns g p m

clearall:
	rm -f ns g p m
	rm -f data.dat data_sorted.dat
	rm -rf p*.dat