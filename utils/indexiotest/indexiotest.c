/****************************************************************
 * file  indexiotest.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Tests if the indextest.h module is working as intended
 * 
****************************************************************/

#include<stdio.h>
#include"pageio.h"
#include"webpage.h"
#include"indexio.h"

/****************************************************************
 * Define macro, globals and helper functions
****************************************************************/
#define eprintf(format, ...) do {                 \
    if (verbose)                                  \
        fprintf(stderr, format, ##__VA_ARGS__);   \
} while(0)
#define verbose 1

// Associates a list of crawled documents with each crawled word
typedef struct {
    char *word;         // The indexed word
    queue_t *doclist;   // List of associated documents
} word_t;

// This struct defines doclist elements
typedef struct {
    int id;             // Id of the crawled page
    int freq;           // Frequency of word in page
} doc_t;

// Frees queue and word in hashtable
void freeWord(void* word) {free(((word_t*)word)->word); }
void freeDoc(void *word) {word_t *w = ((word_t*)word); 
                            qclose(w->doclist); }


/****************************************************************
 * Simple difference comparison
****************************************************************/
int main() {

    hashtable_t *index;

    // Tests index save and load
    index = indexload(".", "indextest.file");
    indexsave(index, ".", "indextest2.file");

    // Cleanup
    happly(index, freeWord);
    happly(index, freeDoc);
    hclose(index);
    return 0;
}
