CC ?= gcc
CFLAGS = -c -DDEBUG=0 -O3 -march=native -DUSEPHRED=0 -DK_LENGTH=8
AR = ar
ARFLAGS = rcs
NAME = quickparse

.PHONY : all
all: lib_quickparse

lib_quickparse: quickparse.c
	$(CC) $(CFLAGS) -o $(NAME).o quickparse.c
	$(AR) $(ARFLAGS) lib$(NAME).a $(NAME).o
	mv lib$(NAME).a ../
	rm -rf $(NAME).o

clean:
	rm -rf ../lib$(NAME).a
