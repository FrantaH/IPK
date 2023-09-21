CC=gcc
CFLAGS=-std=gnu99 -Wall -W -Wextra -pedantic -g -Werror
RM=rm -f

all: ipk-mtrip

ipk-client: ipk-mtrip.c
	$(CC) -o ipk-mtrip ipk-mtrip.c $(CFLAGS)

.PHONY: clean
clean:
	$(RM) *.o ipk-mtrip

