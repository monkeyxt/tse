# Makefile for pageiotest.c
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 31, 2021

CFLAGS=-Wall -pedantic -std=c11 -I ../ -L ../../lib -g
LIBS=-lutils -lcurl

all: pageiotest

pageiotest:
	gcc $(CFLAGS) pageiotest.c $(LIBS) -o $@

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
memtest: pageiotest
	$(VALGRIND) ./pageiotest

clean:
	rm pageiotest 1