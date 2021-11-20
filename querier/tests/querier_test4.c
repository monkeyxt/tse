/****************************************************************
 * file   querier.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   November 8, 2021
 * 
 * Implementation of a simple querier in c.
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

const int32_t maxchar = 128;

// This struct defines the queue of documents ranked
typedef struct {
    int id;
    int rank;
} query_t;

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

// Global hashtable for index
hashtable_t *index;


/****************************************************************
 * NormalizeWord - converts words to lowercase and discard words
 * that are contains non-alphabets and words that has a length
 * less than 3. Word is deleted if it does not fit criteria.
 * \param buffer      The buffer to be normalized
 * \return          Normalized buffer or NULL string
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
    return;
}


/****************************************************************
 * VaidInput - Checks if input is valid. 10 is line break. 9 is
 * horizontal tab. 32 is space.
 * \param buffer      The buffer to be checked
 * \return          TRUE if valid and FALSE otherwise
****************************************************************/
bool ValidInput(char *prev, char *buffer){
    
    char curr[64];
    strcpy(curr, buffer);
    if(strcmp(curr, "and") == 0 || strcmp(curr, "or") == 0) {
        if(strcmp(prev, "and") == 0 || strcmp(prev, "or") == 0 || strcmp(prev, "") == 0) {
            return false;
        }
    }

    return true;
}

bool checkAlpha(char *buffer) {
    char *p = buffer;
    while((int)(*p) != 10) {
        if(!isalpha(*p) && !(((int)(*p) == 9) || ((int)(*p) == 32))){
            return false;
        }
        p++;
    }
    return true;
}


/****************************************************************
 * Hash and queue search functions
****************************************************************/
bool fwd(void *indexw, const void *target) {
  return strcmp(((word_t*)indexw)->word, (char*)target) == 0;
}
bool frk(void *doc, const void *rank) {
    return ((query_t*)doc)->rank == *((int*)rank);
}
bool fid(void *doc, const void *id) {return ((doc_t*)doc)->id == *((int*)id);}


/****************************************************************
 * gdoc - Takes a queue of words and hands back a queue of
 * query_t for documents that contains all of the words that
 * are getting queried.
 * \param words     queue of words to be included
 * \return docs     queue_t of query_t for documents that
 *                  includes all of the words 
****************************************************************/
queue_t *gdoc(queue_t *words) {

    queue_t *ranklist = qopen();
    bool flag = false;

    // Set seed documents to loop through
    char *currword;
    if((currword = (char*)qget(words)) != NULL) {

        word_t* entry = hsearch(index, fwd, currword, strlen(currword));

        if(entry != NULL) {
            
            // If word found in the index hashtable, copy over
            queue_t *copy = qopen();
            doc_t *currdoc;

            while((currdoc = (doc_t*)qget(entry->doclist)) != NULL) {
                if(currdoc->freq > 0) {
                    query_t *docrank = malloc(sizeof(query_t));
                    docrank->id = currdoc->id;
                    docrank->rank = currdoc->freq;
                    qput(ranklist, docrank);
                }
                qput(copy, currdoc);
            }

            qclose(entry->doclist);
            entry->doclist = copy;
        }
        else {
            flag = true;
        }
        free(currword);
    }

    // Eliminate documents without the current word
    while((currword = (char*)qget(words)) != NULL && flag == false) {
        word_t* entry = hsearch(index, fwd, currword, strlen(currword));
        
        // If word exists in hashtable of index
        if(entry != NULL) {
            query_t *currrank;
            queue_t *copy = qopen();

            // Cycle through the current ranklist
            while((currrank = (query_t*)qget(ranklist)) != NULL) {
                
                // Check to see if the current document exists under the entry
                // of the current index search word
                doc_t *d = (doc_t*)qsearch(entry->doclist, fid, &(currrank->id));
                
                // if the current document exists under the indexed word
                if(d != NULL) {
                    int freq = d->freq;
                    if(freq > 0) {
                        if(freq < currrank->rank) currrank->rank = freq;
                        qput(copy, currrank);
                    }
                    else {
                        // Word does not exist in this document, eliminate
                        free(currrank);
                    }
                }
            }

            // Copy structure back
            qclose(ranklist);
            ranklist = copy;
        }
        else {
            flag = false;
        }

        free(currword);
    }

    // Return an empty list if words do not match
    if(flag == true) {
        qclose(ranklist);
        ranklist = qopen();
    }

    return ranklist;
}


/****************************************************************
 * pdoc - Prints the query_t structure
 * \param docs      The query_t structure in void* type
****************************************************************/
void prank(void *docs) {
    query_t *d = (query_t*)docs;
    webpage_t *page = pageload(d->id, "../pages");
    printf("rank: %d doc: %d URL: %s\n", d->rank, d->id, webpage_getURL(page));
    webpage_delete(page);
    return;
}


/****************************************************************
 * sdoc - Sort query_t structure by decreasing rank
 * \param docs      The query_t structure to be sorted
 * \return          Sorted query_t structure
****************************************************************/
void sdoc(queue_t *docs) {

    if(docs == NULL) return;
    int mrank = 0;

    queue_t * copy = qopen();
    query_t *marker;
    query_t *hit;

    // Find the largest rank
    while((marker = qget(docs)) != NULL) {
        if(marker->rank > mrank) mrank = marker->rank;
        qput(copy, marker);
    }

    // Sort from decreasing rank
    while((marker = qget(copy)) != NULL) {
        qput(copy, marker);
        while((hit = (query_t*)qremove(copy, frk, &mrank)) != NULL) {
            qput(docs, hit);
        }
        mrank--;
    }
    qclose(copy);
}



/****************************************************************
 * CombineRank - Combine the rankings of documents given an 'or'
 * statement
 * \param result    The queue of query_t structure to be returned
 *                  to the user
 * \param new       A queue of quert_t used to adjust rankings
 *                  of the query results
 * \return          Updated queue of query_t
****************************************************************/
void CombineRank(queue_t *result, queue_t *new) {

    query_t *buffer;
    while((buffer = (query_t*)qget(new)) != NULL) {

        query_t *q;
        // Insert into result if current query does not exits
        if((q = qsearch(result, fid, &(buffer->id))) == NULL) {
            qput(result, buffer);
        }
        else {
            q->rank += buffer->rank;
            free(buffer);
        }
    }
}


/****************************************************************
 * Querier - returns tokenized words
****************************************************************/
int main(int argc, char *argv[]){

    index = indexload("../indexer", "index.file");

    while(true) {
        
        char *input = (char*)malloc(sizeof(char*)*maxchar);
        char buffer[64] = ""; char prev[64] = "";

        bool valid = true;

        printf(">");
        char* res = fgets(input, 100, stdin);

        // Exit loop if ctrl+D
        if(res == NULL) {
            free(input);
            printf("\n");
            break;
        }

        // Ignore empty input
        if((int)(*input) == 10) {
            free(input);
            continue;
        }

        // Ignore invalid input
        if(!checkAlpha(input)) {
            printf("[invalid query]\n");
            free(input);
            continue;
        }

        char *curr = input;
        queue_t *words = qopen();
        queue_t *results = qopen();

		while(true){

        	while(((int)(*curr)) == 9 || ((int)(*curr)) == 32) curr++;
			
            if(sscanf(curr, "%s", buffer) == 1){
				
                curr += strlen(buffer);
				NormalizeWord(buffer);

                // Check for invalid inputs
                if(!ValidInput(prev, buffer)) {
                    valid = false;
                    printf("[invalid query]\n");
                    break;
                }
                
                // Extract valid word from index and put into queue 
                if(strcmp(buffer, "and") != 0) {

                    if(strcmp(buffer, "or") != 0 || strlen(buffer) > 3) {
                        char *w = (char*)malloc(sizeof(char*) * 128);
                        strcpy(w, buffer);
                        qput(words, w);
                    }
                    else if(strcmp(buffer, "or") == 0) {

                        // If the current word is 'or'
                        queue_t *docs = gdoc(words);
                        CombineRank(results, docs);
                        qclose(docs);
                    }
                }
			}
			else{
				break;
            }

            // Remember previous word
            strcpy(prev, buffer);		
        }

        // Consider trailing query
        if(!ValidInput(buffer, "or") && valid) {
            printf("[invalid query]\n");
            valid = false;
        }

        // Upadate result one last time 
        if(valid) {
            queue_t *docs = gdoc(words);
            CombineRank(results, docs);
            qclose(docs);
            sdoc(results);
            qapply(results, prank);
        }
        

        qclose(words);
        qclose(results);
        free(input);
    }

    happly(index, freeWord);
    happly(index, freeDoc);
    hclose(index);
    return 0;
}