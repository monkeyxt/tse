/****************************************************************
 * file   indexer.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Implementation of a simple indexer in c.
 * 
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<ctype.h>

#include"webpage.h"
#include"hash.h"
#include"queue.h"
#include"pageio.h"
#include"indexio.h"


/****************************************************************
 * Define macro, structs, globals and helper functions
****************************************************************/
#define eprintf(format, ...) do {                 \
    if (verbose)                                  \
        fprintf(stderr, format, ##__VA_ARGS__);   \
} while(0)
#define verbose 1

// This struct associates a list of crawled documents with each crawled word
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
void freeDoc(void *word) {word_t *w = ((word_t*)word); qclose(w->doclist); }

// Sum of word occurrences
int sum = 0;


/****************************************************************
 * Private Helper Functions: hash and queue search function to 
 * match word_t words
 * \param p     object pointer
 * \param s     target 
 * \return      1 if word found, 0 otherwise
****************************************************************/
bool hsearchfn(void *p, const void *s) {
    word_t *p_word = (word_t*)p;
    char *s_word = (char*)s;
    return !(strcmp(p_word->word, s_word));
}

bool qsearchfn(void *p, const void *s){
    doc_t *d = (doc_t*)p;
    int *id = (int*)s;
    return (d->id == *id);
}


/****************************************************************
 * Private Helper Functions: hash and queue apply function to sum 
 * word frequency given a doc_t type. Results are stored in the 
 * global variable sum.
 * 
 *  hashtable --> elements (word_t) --> queues of doc_t
****************************************************************/
void qsumfn(void *p) {
    doc_t *p_doc = (doc_t*)p;
    sum += p_doc->freq;
}

void hsumfn(void *p) {
    word_t *p_word = (word_t*)p;
    qapply(p_word->doclist, &qsumfn);
}


/****************************************************************
 * NormalizeWord - converts words to lowercase and discard words
 * that are contains non-alphabets and words that has a length
 * less than 3. Word is deleted if it does not fit criteria.
 * \param word      The word to be normalized
 * \return          Normalized word or NULL string
****************************************************************/
void NormalizeWord(char *word){
    int pointer = 0;
    while(word[pointer]){
        
        // Convert words to lower case
        if(isalpha(word[pointer])) {
            word[pointer] = tolower(word[pointer]);
            pointer++;
        }
        else{
            *word = '\0'; // Make the word a NULL character
            return;
        }
    }
    if(pointer < 3) *word = '\0';
    return;
}


/****************************************************************
 * cpage - checks validity of URL and creates a webpage
 * \param word      The indexing word
 * \return          word_t pointer to hand back
****************************************************************/
word_t *cword(char *word) {
    word_t *w = (word_t*)malloc(sizeof(word_t));
    w->word = word;
    w->doclist = qopen();
    return w;
}


/****************************************************************
 * cpage - checks validity of URL and creates a webpage
 * \param id        doc ID
 * \param freq      doc word frequency
 * \return          doc_t pointer to hand back
****************************************************************/
doc_t *cdoc(int id, int freq) {
    doc_t *d = (doc_t*)malloc(sizeof(doc_t));
    d->id = id;
    d->freq = freq;
    return d;
}


/****************************************************************
 * Indexer - indexes pages by words
 * \param index     Hashtable to store index info
 * \param page      The page to be indexed
 * \param id        ID of the page to be indexed
 * \param indexnm   Name of index file
 * 
 * \return          Quietly outputs index to file
****************************************************************/
void indexer(hashtable_t *index, webpage_t *page, int id, char *indexnm){

    int pos = 0;
    char *word = NULL;
    while((pos = webpage_getNextWord(page, pos, &word)) > 0) {
        
        NormalizeWord(word);
        if(strlen(word) > 0) {

            word_t *w;
            // Put the word into hashtable if it does not exist
            if((w = hsearch(index, &hsearchfn, word, strlen(word))) == NULL) {
                w = cword(word);
                doc_t *d = cdoc(id, 1);
                qput(w->doclist, d);
                hput(index, w, w->word, strlen(w->word));
            }
            else {
                doc_t *d;
                // Insert doc into queue if doc does not exist
                if((d = qsearch(w->doclist, &qsearchfn, &id)) == NULL) {
                    doc_t *d = cdoc(id, 1);
                    qput(w->doclist, d);
                }
                else {
                    d->freq++;
                }
                free(word);
            }

        }
        else {
            // Also free NULL character word
            free(word);     
        }
    }

    // Cleanup
    webpage_delete(page);
    return;
}


/****************************************************************
 * checkinput - checks the cmd input provided by the user
 * \return error code:
 *              0: parsing success
 *              1: invalid number of arguments
 *              2: invaild pagedir
****************************************************************/
int checkinput(int argc, char* argv[]) {
    
    // Parse the cmdline inputs
    if(argc != 3) {
        printf("usage: indexer <pagedir> <indexnm>\n");
        return 1;
    }

    // Make sure that directory is valid
    struct stat st = {0};
    char *dirname = argv[1];
    if(stat(dirname, &st) == -1) {
        printf("Error: invalid pagedir\n");
        return 2;
    }

    return 0;
}


/****************************************************************
 * Indexer - indexes pages by words
 * usage: indexer <pagedir> <indexnm>
****************************************************************/
int main(int argc, char *argv[]){
    
    int error = checkinput(argc, argv);
    if(error != 0) {
        eprintf("Error parsing arguments: %d\n", error);
        exit(EXIT_FAILURE);
    }

    // Index all the pages in argv[1]
    hashtable_t *index = hopen(1000);
    int id = 1;
    webpage_t *page = pageload(id, argv[1]);
    while(page != NULL) {
        printf("Indexing page %d...\n", id);
        indexer(index, page, id, argv[2]);
        id++;
        page = pageload(id, argv[1]);
    }

    printf("Indexing compete...saving index to local...\n");
    indexsave(index, ".", argv[2]);

    // Clean up
    happly(index, freeWord);
    happly(index, freeDoc);
    hclose(index);
    return 0;
}