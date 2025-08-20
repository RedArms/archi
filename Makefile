.RECIPEPREFIX = >
CC=gcc
CFLAGS=-O3 -mavx -std=c11 -Wall -Wextra
OBJ=main.o matrix.o utils.o

prog: $(OBJ)
>$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
>$(CC) $(CFLAGS) -c $< -o $@

clean:
>rm -f $(OBJ) prog
