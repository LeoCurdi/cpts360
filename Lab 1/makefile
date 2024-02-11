CC = gcc
CFLAGS = -I. -g

name = lab1_Curdi

OBJS = $(name).o commands.o

$(name): $(OBJS)
	$(CC) -o $(name) $(OBJS)

clean: rm $(name) *.o