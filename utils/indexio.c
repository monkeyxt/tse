/****************************************************************
 * file   indexio.h
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Module that saved and loads indexes : indexsave() and 
 * indexload() saves and load an index to a named file indexnm. 
 * The index file contains one line for each word in the index. 
 * Each line has the format: 
 *
 * <word> <docID1> <count1> <docID2> <count2>..<docIDN> <countN> 
 *
 * where <word> is a string of lowercase letters, <docIDi> is a 
 * positive integer designating a document, <counti> is a 
 * positive integer designating the number of occurrences of 
 * <word> in <docIDi>; each entry should be placed on the line 
 * separated by a space. 
 * 
****************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <errno.h>

#include "webpage.h"
#include "hash.h"
#include "queue.h"


/****************************************************************
 * Define macro, structs, globals and helper functions
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

// Global file output
FILE *outputf;

// Buffer size for reading
const int buffer_size = 128;


/****************************************************************
 * Custom printer functions. pword() prints all words and calls
 * pdoc() to print all elements->id and elements->freq in queue.
****************************************************************/
void pdoc(void *p){
    doc_t *doc = (doc_t*)p;
    fprintf(outputf, "%d %d ", doc->id, doc->freq);
}

void pword(void *p){
    word_t *word = (word_t*)p;
    fprintf(outputf, "%s ", word->word);
    qapply(word->doclist, &pdoc);
    fprintf(outputf, "\n");
}


/****************************************************************
 * indexsave - Saves index table to file
 * \param htp       Index table to be saved (hashtable_t)
 * \param dirname   Directory for saved index file (char *)
 * \param indexnm   Name of the saved index file (char *)
 * 
 * \return          0 if sucess and -1 otherwise
****************************************************************/
int32_t indexsave(hashtable_t* htp, char* dirname, char* indexnm) {
    if(htp == NULL) return -1;
    char filename[128];
    sprintf(filename, "%s/%s", dirname, indexnm);
    outputf = fopen(filename, "w");

    // Add some safety  signature
    if(outputf == NULL) {
        eprintf("Failed to open file %s: error %d\n", filename,
                    errno);
        return -1;
    }
    if(access(filename, W_OK) != 0) {
        eprintf("Failed to access file %s: error %d\n", filename,
                    errno);
        return -1;
    }

    happly(htp, &pword);
    fclose(outputf);
    return 0;
}


/****************************************************************
 * indexload - Loads index from file to hashtable
 * \param dirname   Directory for saved index file (char *)
 * \param indexnm   Name of the saved index file (char *)
 * 
 * \return htp      A hashtable of loaded index
****************************************************************/
hashtable_t* indexload(char* dirname, char* indexnm) {
    
    hashtable_t *h = hopen(1000);
    char filename[128];
    sprintf(filename, "%s/%s", dirname, indexnm);
    FILE *inputf = fopen(filename, "r");

    // Add some safety signatures
    if(inputf == NULL) {
        eprintf("Failed to open file %s: error %d\n", filename,
                    errno);
        return NULL;
    }
    if(access(filename, R_OK) != 0) {
        eprintf("Failed to access file %s: error %d\n", filename,
                    errno);
        return NULL;
    }

    // Scan index file
    char buffer[buffer_size];
    while(fscanf(inputf, "%s", buffer) == 1) {

        // Make word_t structure
        word_t *w = (word_t*)malloc(sizeof(word_t));
        w->word = (char*)malloc(sizeof(char) * strlen(buffer) + 1);
        w->doclist = qopen();

        // Put elements into word_t struct
        strcpy(w->word, buffer);
        int id, freq;
        while(fscanf(inputf, "%d %d", &id, &freq) == 2) {
            // Put doc_id and word frequency into struct
            doc_t *doc = (doc_t*)malloc(sizeof(doc_t));
            doc->id = id;
            doc->freq = freq;
            qput(w->doclist, doc);
        }

        // Insert word_t into hashtable
        hput(h, w, w->word, strlen(w->word));
    }

    fclose(inputf);
    return h;
}
