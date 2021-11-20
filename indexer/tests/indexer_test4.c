/****************************************************************
 * file   indexer_test4.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Added indexer argument for page ids
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
 * Private helper function: Convert string into unsigned 32 bit
 * integer. 
 * \param s     The string to be converted
 * \return x    An unsigned 32-bit integer
****************************************************************/
static uint32_t convert_uint(const char *s) {
    long long x;
    char *pEnd;
    x = strtoul(s, &pEnd, 10);
    return x;
}


/****************************************************************
 * Private Helper Functions: hash search function to match 
 * word_t words
 * \param p     word_t pointer
 * \param s     target word (string)
 * \return      1 if word found, 0 otherwise
****************************************************************/
bool hsearchfn(void *p, const void *s) {
    word_t *p_word = (word_t*)p;
    char *s_word = (char*)s;
    return !(strcmp(p_word->word, s_word));
}


/****************************************************************
 * Private Helper Functions: queue search function to match 
 * doc_t queue ids
 * \param p     doc_t pointer
 * \param s     target id (int)
 * \return      1 if word found, 0 otherwise
****************************************************************/
bool qsearchfn(void *p, const void *s){
    doc_t *d = (doc_t*)p;
    int *id = (int*)s;
    return (d->id == *id);
}


/****************************************************************
 * Private Helper Functions: queue apply function to sum word
 * frequency given a doc_t type. Results are stored in the 
 * global variable sum.
 * \param p     The target doc_t type
****************************************************************/
void qsumfn(void *p) {
    doc_t *p_doc = (doc_t*)p;
    sum += p_doc->freq;
}


/****************************************************************
 * Private Helper Functions: hash apply function to sum the
 * queues in all elements of the hashtable
 * 
 * hashtable --> elements (word_t) --> queues of doc_t
 * \param p     The target doc_t type
****************************************************************/
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
 * Indexer - indexes pages by words
 * \param id    The ending ID of pages to index (int)
 * \return      Number of words indexed
****************************************************************/
int indexer(int id) {

    hashtable_t *index = hopen(1000);
    webpage_t *test = pageload(id, "../../pages");

    int pos = 0;
    char *word = NULL;
    while((pos = webpage_getNextWord(test, pos, &word)) > 0) {
        NormalizeWord(word);

        // If the word is valid
        if(strlen(word) > 0) {

            // Put the word into hashtable if it does not exist
            word_t *w;
            if((w = hsearch(index, &hsearchfn, word, strlen(word))) == NULL) {
                w = (word_t*)malloc(sizeof(word_t));
                w->word = word;
                w->doclist = qopen();

                // Make document & insert into queue
                doc_t *d = malloc(sizeof(doc_t));
                d->id = id;
                d->freq = 1;
                qput(w->doclist, d);
                hput(index, w, w->word, strlen(w->word));

            }
            else {
                // Insert the document into queue if the document does not exit
                doc_t * d;
                if((d = qsearch(w->doclist, &qsearchfn, &id)) == NULL) {
                    d = (doc_t*)malloc(sizeof(doc_t));
                    d->id = id;
                    d->freq = 1;
                    qput(w->doclist, d);
                }
                else {
                    d->freq++;
                }
                free(word);
            }
        }
        else {
            free(word);
        }
    }

    // Print sum of words
    happly(index, &hsumfn);
    eprintf("Hashtable sum: %d\n", sum);

    // Cleanup
    webpage_delete(test);
    happly(index, freeWord);
    happly(index, freeDoc);
    hclose(index);
    return 0;
}


/****************************************************************
 * Indexer - indexes pages by words
 * usage: indexer <pagedir> <indexnm>
****************************************************************/
int main(int argc, char *argv[]){

    if (argc < 1) {
        exit(EXIT_FAILURE);
    }
    
    if(argv[1] != NULL) {
        int id = convert_uint(argv[1]);
        indexer(id);
    }
    return 0;

}