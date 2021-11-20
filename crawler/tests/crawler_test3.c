/****************************************************************
 * file   crawler_test3.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 16, 2021
 * 
 * Tests the crawler hashtable functions
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

bool searchfn(void *p, const void *s) {
	char *p_url = (char*)p ;
	char *s_url = (char*)s;
    return !strcmp(p_url, s_url);
}

int main() {

    char *seedURL = "https://thayer.github.io/engs50/";
    int depth = 0;
    int max_depth = 5;
    int max_breath = 50;
    webpage_t *seed = webpage_new(seedURL, depth, NULL);

    if(!webpage_fetch(seed)) {
        fprintf(stderr, "Failed fetching seed page\n");
        exit(EXIT_FAILURE);
    }

    // Initialize queue for list of URLs
    queue_t *u_queue = qopen();
    if(u_queue == NULL) {
        fprintf(stderr, "Error: Failed to initialize queue\n");
        exit(EXIT_FAILURE);
    }

    // Initalize hashtable to store visited URLs. The hashtable assumes a
    // maximum of 50 URLs on each webpage.
    hashtable_t *vis = hopen((max_depth + 1) * max_breath);
    if(vis == NULL) {
        fprintf(stderr, "Error: Failed to initalize vis hashtable\n");
        exit(EXIT_FAILURE);
    }

    // Add all urls to the u_queue
    int pos = 0;
    char *url = NULL;
    while((pos = webpage_getNextURL(seed, pos, &url)) > 0) {
        if(NormalizeURL(url) && IsInternalURL(url)) {

            webpage_t *page = webpage_new(url, depth + 1, NULL);

            // Insert url into hashtable
            if(hsearch(vis, &searchfn, url, sizeof(url)) == NULL) {
                hput(vis, url, url, sizeof(url));
                qput(u_queue, page);
            }
            else {
                webpage_delete(page);
                free(url);
            }
        }
    }
    free(url);

    // Print everything in queue and check if the links are internal
    webpage_t *page = (webpage_t*)qget(u_queue);
    while(page != NULL) {
        if(!IsInternalURL(webpage_getURL(page))) {
            fprintf(stderr, "Error: queue contains external URL\n");
            exit(EXIT_FAILURE);
        }

        printf("URL: %s\nInternal: %s\n", webpage_getURL(page), 
                        IsInternalURL(webpage_getURL(page)) ? "Y" : "N");
        printf("-------------------------------------------\n");

        webpage_delete(page);
        page = (webpage_t*)qget(u_queue);
    }
    
    // Free seed page
    webpage_delete(seed);

    // Close queue and hashtable
    qclose(u_queue);
    hclose(vis);

    exit(EXIT_SUCCESS);
    
}