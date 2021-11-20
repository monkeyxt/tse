# Makefile for indexiotest.c
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 31, 2021

CFLAGS=-Wall -pedantic -std=c11 -I ../ -L ../../lib -g
LIBS=-lutils -lcurl

all: indexiotest

indexiotest:
	gcc $(CFLAGS) indexiotest.c $(LIBS) -o $@

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
memtest: indexiotest
	$(VALGRIND) ./indexiotest

difftest: indexiotest
	./indexiotest
	diff indextest.file indextest2.file

clean:
	rm indexiotest indextest2.file