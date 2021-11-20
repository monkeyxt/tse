/****************************************************************
 * file   indexer_test1.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 18, 2021
 * 
 * Tests if the indexer prints and normalizes word from a HTML
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
            *word = '\0';
            return;
        }
    }
    if(pointer < 3) *word = '\0';
    return;
}

int main(){
    
    webpage_t *test = pageload(1, "../../pages");
    
    int pos = 0;
    char *word = NULL;

    while((pos = webpage_getNextWord(test, pos, &word)) > 0) {
        NormalizeWord(word);
        if(strlen(word) > 0) {
            printf("%s\n", word);
        }
        free(word);
    }

    webpage_delete(test);
    return 0;
}