myasm: main.o first_pass.o second_pass.o input_functions.o linked_lists.o main.h first_pass.h second_pass.h utils.h
	gcc -g -ansi -Wall -pedantic main.o first_pass.o second_pass.o input_functions.o linked_lists.o -o myasm

main.o: main.c utils.h main.h
	gcc -c -g -ansi -Wall -pedantic main.c -o main.o

first_pass.o: first_pass.c utils.h first_pass.h
	gcc -c -g -ansi -Wall -pedantic first_pass.c -o first_pass.o

second_pass.o: second_pass.c utils.h second_pass.h
	gcc -c -g -ansi -Wall -pedantic second_pass.c -o second_pass.o

input_functions.o: input_functions.c utils.h first_pass.h
	gcc -c -g -ansi -Wall -pedantic input_functions.c -o input_functions.o

linked_lists.o: linked_lists.c utils.h
	gcc -c -g -ansi -Wall -pedantic linked_lists.c -o linked_lists.o
