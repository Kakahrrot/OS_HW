all: opt.c Bench.c generate.c
	gcc -o opt opt.c
	gcc -o Bench Bench.c
	gcc -o generate generate.c
clear:
	rm input.txt output.txt sort generate 
