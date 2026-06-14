cflags= -lraylib -lm -Wall -Wextra -pedantic -ggdb

make:
	gcc -o imgv main.c $(cflags)
