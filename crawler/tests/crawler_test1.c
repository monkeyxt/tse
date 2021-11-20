/****************************************************************
 * file   crawler_test1.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 16, 2021
 * 
 * Tests if the crawler downloads a page successfully
 * 
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include"../../utils/webpage.h"
#include"../../utils/hash.h"
#include"../../utils/queue.h"

int main() {

    char *seedURL = "https://thayer.github.io/engs50/";
    webpage_t *seed = webpage_new(seedURL, 0, NULL);
    bool rv = webpage_fetch(seed);

    if(!rv) {
        printf("Failed fetching seed page\n");
        exit(EXIT_FAILURE);
    }

    // Print all urls in the fetched page
    int pos = 0;
    char *url;
    while((pos = webpage_getNextURL(seed, pos, &url)) > 0) {
        printf("URL: %s\nInternal: %s\n", url, 
                        IsInternalURL(url) ? "Y" : "N");
        printf("-------------------------------------------\n");
        free(url);
    }
    
    // Free seed page
    webpage_delete(seed);
    exit(EXIT_SUCCESS);
    
}