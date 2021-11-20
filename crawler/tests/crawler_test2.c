/****************************************************************
 * file   crawler_test2.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 16, 2021
 * 
 * Tests if the crawler saves page files in the queue correctly
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
    int depth = 0;

    webpage_t *seed = webpage_new(seedURL, depth, NULL);
    bool rv = webpage_fetch(seed);

    if(!rv) {
        printf("Failed fetching seed page\n");
        exit(EXIT_FAILURE);
    }

    // Open queue for list of URLs
    queue_t *u_queue = qopen();
    int pos = 0;
    char *url;
    while((pos = webpage_getNextURL(seed, pos, &url)) > 0) {
        if(IsInternalURL(url)) {
            webpage_t *page = webpage_new(url, depth + 1, NULL);
            qput(u_queue, page);
        }
        free(url);
    }

    // Print everything in queue
    webpage_t *page = (webpage_t*)qget(u_queue);
    while(page != NULL) {
        if(!IsInternalURL(webpage_getURL(page))) {
            printf("Error: queue contains external URL\n");
            exit(EXIT_FAILURE);
        }

        printf("URL: %s\nInternal: %s\n", webpage_getURL(page), 
                        IsInternalURL(webpage_getURL(page)) ? "Y" : "N");
        printf("-------------------------------------------\n");

        webpage_delete(page);
        page = (webpage_t*)qget(u_queue);
    }
    
    // Free seed page
    qclose(u_queue);
    webpage_delete(seed);
    exit(EXIT_SUCCESS);
    
}