/****************************************************************
 * file   indexer_test2.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Tests the hashing function for the indexer
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


typedef struct {
    char *word;
    int freq;
} wordcount_t;

int sum = 0;
void freeWord(void* word) {free(((wordcount_t*)word)->word);}


/****************************************************************
 * Search function to match words
 * \param p     The wordcount strut of a given word
 * \param s     The target search word
 * \return      1 if words match and 0 otherwise
****************************************************************/
bool searchfn(void *p, const void *s) {
    wordcount_t *p_word = (wordcount_t*)p;
    char *s_word = (char*)s;
    return !(strcmp(p_word->word, s_word));
}


/****************************************************************
 * Sum function to calculate all words occurences
 * \param p     The wordcount struct to be counted
****************************************************************/
void sumfn(void *p) {
    wordcount_t *p_word = (wordcount_t*)p;
    sum += p_word->freq;
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
        if(isalpha(word[pointer])) {
            word[pointer] = tolower(word[pointer]);
            pointer++;
        }
        else{
            // Make the word a NULL character
            *word = '\0';
            return;
        }
    }
    if(pointer < 3) *word = '\0';
    return;
}


int main(){
    
    hashtable_t *index = hopen(1000);
    webpage_t *test = pageload(1, "../../pages");

    int pos = 0;
    char *word = NULL;
    while((pos = webpage_getNextWord(test, pos, &word)) > 0) {
        NormalizeWord(word);

        // If the word is valid
        if(strlen(word) > 0) {
            // Put the word into hashtable if it does not exist
            wordcount_t *w;
            if((w = hsearch(index, &searchfn, word, strlen(word))) == NULL) {
                w = (wordcount_t*)malloc(sizeof(wordcount_t));
                w->freq = 1;
                w->word = word;
                hput(index, w, w->word, strlen(w->word));

            }
            else {
                // Increase the count of the indexed word
                w->freq++;
                free(word);
            }
        }
        else {
            free(word);
        }
    }

    // Print sum of words
    happly(index, &sumfn);
    eprintf("Hashtable sum: %d\n", sum);

    // Cleanup
    webpage_delete(test);
    happly(index, freeWord);
    hclose(index);
    return 0;
}