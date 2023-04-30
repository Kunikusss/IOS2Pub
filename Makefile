CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LDLIBS = -lm

.PHONY: clean all

all: output
	gcc -std=gnu99 -Wall -Wextra -pedantic main.c -o proj2\
	
output:
	touch proj2.out
#$(CC) $(CFLGS) main.c -o proj2.out

run:
	./proj2
	cat proj2.out	


clean:
	rm -f *.o
	rm -f *.out
	rm -f *.d
	rm -f *.log

