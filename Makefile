CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = problem11.o problem12.o

all: problem1


problem11: problem11.o
	$(CC) -o ./Excercises/problem11 ./Excercises/problem11.o -lm

problem11.o: problem1-1.c
	$(CC) $(CFLAGS) -c ./Excercises/problem1-1.c

problem12: problem12.o
	$(CC) -o ./Excercises/problem12 ./Excercises/problem12.o -lm

problem12.o: problem1-2.c
	$(CC) $(CFLAGS) -c ./Excercises/problem1-2.c

clean:
	rm -f calc $(OBJ)