CC = gcc 
CFLAGS = -std=c17 -Wall -Wextra -Werror -Wpedantic -g

all: httpserver

httpserver: httpserver.o bind.o
	$(CC) -o httpserver httpserver.o bind.o -lm

httpserver.o: httpserver.c
	$(CC) $(CFLAGS) -c httpserver.c
bind.o: bind.c
	$(CC) $(CFLAGS) -c bind.c

clean:
	rm -f httpserver httpserver.o bind.o 

scan-build: clean
	scan-build make   
	
format: 
	clang-format -i -style=file httpserver.c 
check:
	valgrind --leak-check=full ./httpserver 1234
	
that: 
	./httpserver a.txt a
