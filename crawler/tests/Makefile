# Makefile for crawler
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 16, 2021

CC = gcc
CFLAGS = -pthread -Wall -pedantic -std=c11 -I../../utils/ -L../../lib/ -g
LIBS = -lutils -lcurl

BUILD_DIR = ../testbuild

all: directories crawler_tests1 crawler_tests2 crawler_tests3 crawler_tests4

directories: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

crawler: crawler.c 
	$(CC) $(CFLAGS) crawler.c $(LIBS) -o $@ 

crawler_tests1: crawler_test1.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

crawler_tests2: crawler_test2.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

crawler_tests3: crawler_test3.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

crawler_tests4: crawler_test4.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/$@ 

clean:
	rm -rf ../testbuild ../../pages
	