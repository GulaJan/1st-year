/*
 * File: Makefile
 * Date: 04.04.2016
 * Author: Ján Gula,xgulaj00
 * Project: IOS-DU2
 * Description: Makefile
*/

CC=gcc

CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -pthread

all: proj2

proj2: proj2.o
	$(CC) $(CFLAGS) proj2.o -o $@
proj2.o: proj2.c proj2.h
	$(CC) $(CFLAGS) -c proj2.c -o $@

.PHONY:
	clean clean-all

clean:
	rm -f *.o

clean-all:
	rm -f *.o proj2 proj2.out

