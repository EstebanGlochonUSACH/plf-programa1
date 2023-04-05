FLAGS = -Wall

main: main.c
	gcc $(FLAGS) -o main main.c

clear:
	rm -f *.o main
