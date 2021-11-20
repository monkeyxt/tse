# Makefile for crawler
#
# Tian Xia (tian.xia.ug@dartmouth.edu) - October 16, 2021

CC = gcc
CFLAGS = -pthread -Wall -pedantic -std=c11 -I utils/ -L lib/ -g
LIBS = -lutils -lcurl

BUILD_DIR = bin

all: directories crawler_o indexer_o querier_o

directories: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

crawler_o: crawler/crawler.c 
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/crawler

indexer_o: indexer/indexer.c 
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/indexer

querier_o: querier/querier.c 
	$(CC) $(CFLAGS) $< $(LIBS) -o $(BUILD_DIR)/querier

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all
memtest: $(BUILD_DIR)/crawler
	$(VALGRIND) .$(BUILD_DIR)/crawler "https://thayer.github.io/engs50/" "../pages/" 2 3

clean:
	rm -rf bin pages

.phony: all clean