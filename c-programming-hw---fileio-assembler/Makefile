all : assembler

assembler : assembler.c asm_parser.o  
	clang -g -o assembler assembler.c asm_parser.o 

asm_parser.o : asm_parser.c asm_parser.h
	clang -g -c asm_parser.c

clean :
	rm -rf *.o
	rm -rf assembler
