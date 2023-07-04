all: arq.c arq.h tipo1.c tipo1.h tipo2.c tipo2.h auxiliar.c auxiliar.h main.c funcoesFornecidas.c funcoesFornecidas.h arvoreB.c arvoreB.h
	gcc -g -c arq.c funcoesFornecidas.c tipo1.c tipo2.c auxiliar.c arvoreB.c
	gcc -g -o exe main.c arq.o tipo1.o tipo2.o auxiliar.o funcoesFornecidas.o arvoreB.o -Wall -Werror -lm

run: exe
	./exe

tipo1: tipo1.c tipo1.h
	gcc -g -c tipo1.c
	gcc -g -o exe main.c arq.o tipo1.o tipo2.o auxiliar.o funcoesFornecidas.o arvoreB.o -Wall -Werror -lm

tipo2: tipo2.c tipo2.h
	gcc -g -c tipo2.c
	gcc -g -o exe main.c arq.o tipo1.o tipo2.o auxiliar.o funcoesFornecidas.o arvoreB.o -Wall -Werror -lm

aux: auxiliar.c auxiliar.h
	gcc -g -c auxiliar.c
	gcc -g -o exe main.c arq.o tipo1.o tipo2.o auxiliar.o funcoesFornecidas.o arvoreB.o -Wall -Werror -lm

arvoreB: arvoreB.c arvoreB.h
	gcc -g -c arvoreB.c
	gcc -g -o exe main.c arq.o tipo1.o tipo2.o auxiliar.o funcoesFornecidas.o arvoreB.o -Wall -Werror -lm

valgrind:
	cp arquivos/antes/binario12.bin binario12.bin
	cp arquivos/antes/indice12.bin indice12.bin
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./exe < 12.in

clean:
	rm *.o exe

zip:
	zip envio *c *h Makefile