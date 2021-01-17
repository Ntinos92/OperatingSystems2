OBJS	= main.o myFuncs1.o
SOURCE	= main.c myFuncs1.c
HEADER	= myHeader1.h
OUT	= program
CC	= gcc
FLAGS	= -g3 -c -Wall

all: $(OBJS)
	$(CC) -g3 $(OBJS) -o $(OUT) -lm

main.o: main.c
	$(CC) $(FLAGS) main.c

myFuncs1.o: myFuncs1.c
	$(CC) $(FLAGS) myFuncs1.c


clean:
	rm -f $(OBJS)

