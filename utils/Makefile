# Makefile for utils
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 16, 2021

CC			:= gcc
CFLAGS		:= -Wall -pedantic -std=c11 -I. -g
LIBS		:= -lm

OFILES=queue.o hash.o webpage.o pageio.o indexio.o lhash.o lqueue.o

BUILD_DIR = ../lib
directories: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

all: $(OFILES)
	ar cr ../lib/libutils.a $(OFILES)

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c $<

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c $<

webpage.o: webpage.c webpage.h
	$(CC) $(CFLAGS) -c $<

pageio.o: pageio.c pageio.h
	$(CC) $(CFLAGS) -c $<

indexio.o: indexio.c indexio.h
	$(CC) $(CFLAGS) -c $<

lhash.o: hash.c hash.h queue.h lhash.h
	gcc $(CFLAGS) -c lhash.c

lqueue.o: lqueue.c lqueue.h queue.h
	gcc $(CFLAGS) -c lqueue.c

clean:
	rm -rf *.o ../lib