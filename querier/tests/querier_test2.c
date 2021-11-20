/****************************************************************
 * file   querier_test2.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   November 8, 2021
 * 
 * Rank one document
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
bool ValidInput(char *buffer){
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
 * Querier - returns tokenized words
****************************************************************/
int main(int argc, char *argv[]){

    while(true) {
        
        char *input = (char*)malloc(sizeof(char*)*maxchar);
        char buffer[64];

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
        if(!ValidInput(input)) {
            printf("[invalid query]\n");
            free(input);
            continue;
        }

        char *curr = input;
        int rank = 1 << 30;

		while(true){
			while(((int)(*curr)) == 9 || ((int)(*curr)) == 32){	
				curr++;
			}

			if(sscanf(curr, "%s", buffer) == 1){
				curr += strlen(buffer);
				NormalizeWord(buffer);

                // Extract word from index
                if(strcmp(buffer, "and") != 0 && strcmp(buffer, "or") != 0
                                                    && strlen(buffer) > 3) {

                    char scanner[128];
                    int counter = 0, id = 0;
                    bool vis = false;

                    FILE *inputf = fopen("../indexer/index.file", "r");

                    while(fscanf(inputf, "%s %d %d", scanner, &id, &counter) != EOF) {
                        if(strcmp(scanner, buffer) == 0) {
                            printf("%s:%d ", buffer, counter);
                            if(counter <= rank) rank = counter;
                            vis = true;
                            break;
                        }
                    }

                    if(!vis){
                        printf("%s:0", buffer);
                        rank = 0;
                    }

                    fclose(inputf);
                }
			}
			else{
				break;
			}
		}

        // If none of the inputs are three or more characters
        if(rank == 1 << 30 && ValidInput(input) && strlen(input) > 0) {
            printf("[invalid query]\n");
            free(input);
            continue;
        }

        // Print smallest rank
        printf("-- %d\n", rank);
        free(input);
    }

    return 0;
}