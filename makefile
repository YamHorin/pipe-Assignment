CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

.PHONY: all clean

all: nicecmp loopcmp


nicecmp: nicecmp.c
	$(CC) $(CFLAGS) -o $@ $<

loopcmp: loopcmp.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f exe4 loopcmp
