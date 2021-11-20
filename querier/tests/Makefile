# Makefile for querier
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 16, 2021

CC = gcc
CFLAGS = -pthread -Wall -pedantic -std=c11 -I../../utils/ -L../../lib/ -g
LIBS = -lutils -lcurl

BUILD_DIR = ../testbuild

all: directories querier_tests1 querier_tests2 querier_tests3 querier_tests4

directories: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

querier_tests1: querier_test1.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

querier_tests2: querier_test2.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

querier_tests3: querier_test3.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

querier_tests4: querier_test4.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

clean:
	rm -rf ../testbuild 