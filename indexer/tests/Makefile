# Makefile for crawler
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 16, 2021

CC = gcc
CFLAGS = -pthread -Wall -pedantic -std=c11 -I../../utils/ -L../../lib/ -g
LIBS = -lutils -lcurl

BUILD_DIR = ../testbuild

all: directories indexer_tests1 indexer_tests2 indexer_tests3 indexer_tests4

directories: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

indexer_tests1: indexer_test1.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

indexer_tests2: indexer_test2.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

indexer_tests3: indexer_test3.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

indexer_tests4: indexer_test4.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

clean:
	rm -rf ../testbuild
	