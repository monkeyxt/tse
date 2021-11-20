# Makefile for lqueuetest.c
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 31, 2021

CFLAGS=-pthread -Wall -pedantic -std=c11 -I ../ -L ../../lib -g
LIBS=-lutils -lcurl

all: lqueuetest

lqueuetest:
	gcc $(CFLAGS) lqueuetest.c $(LIBS) -o $@

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
memtest: lqueuetest
	$(VALGRIND) ./lqueuetest

runtest: lqueuetest
	bash runtest.sh ./lqueuetest

clean:
	rm lqueuetest