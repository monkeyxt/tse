/****************************************************************
 * file   crawler_test4.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 16, 2021
 * 
 * Tests if the crawler saves the a page file correctly
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


const int max_id = 16;
struct stat st = {0};

bool searchfn(void *p, const void *s) {
	char *p_url = (char*)p ;
	char *s_url = (char*)s;
    return !strcmp(p_url, s_url);
}

int32_t pagesave(webpage_t *pagep, int id, char *dirname) {
    
    // Extract information to save
    char* html  = webpage_getHTML(pagep);
    int   len   = webpage_getHTMLlen(pagep);
    char* url   = webpage_getURL(pagep);
    int   depth = webpage_getDepth(pagep);

    // Populate filename
    char *filename = malloc(sizeof(char)*strlen(dirname) + sizeof(char) * 16);
    sprintf(filename, "%s/%d", dirname, id);

    // Make directory if does not exist
    if(stat(dirname, &st) == -1) mkdir(dirname, 0777);
    if(access(dirname, W_OK)) chmod(dirname, W_OK);

    // Make page file
    FILE *outputf = fopen(filename, "w");
    if(outputf == NULL) {
        fprintf(stderr, "Error: create page file failed\n");
        printf("Error %d \n", errno);
        return -1;
    }
    fprintf(outputf, "%s\n%d\n%d\n%s\n", url, depth, len, html);
    fclose(outputf);

    free(filename);
    return 0;
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

    // TEST: save the seed page
    if(pagesave(seed, 0, "../pages") == -1) {
        fprintf(stderr, "Error: failed to save seed page\n");
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