# Makefile for lhashtest.c
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 31, 2021

CFLAGS=-pthread -Wall -pedantic -std=c11 -I ../ -L ../../lib -g
LIBS=-lutils -lcurl

all: lhashtest

lhashtest:
	gcc $(CFLAGS) lhashtest.c $(LIBS) -o $@

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
memtest: lhashtest
	$(VALGRIND) ./lhashtest

runtest: lhashtest
	bash runtest.sh ./lhashtest

clean:
	rm lhashtest