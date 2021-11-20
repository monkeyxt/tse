/****************************************************************
 * file   pageiotest.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Tests if the pageio.h module is working as intended
 * 
****************************************************************/

#include<stdio.h>
#include "pageio.h"
#include "webpage.h"

/****************************************************************
 * Define macro, globals and helper functions
****************************************************************/
#define eprintf(format, ...) do {                 \
    if (verbose)                                  \
        fprintf(stderr, format, ##__VA_ARGS__);   \
} while(0)
#define verbose 1


/****************************************************************
 * Simple difference comparison
****************************************************************/
int main(){

    webpage_t *original = pageload(1, "../../pages");
    pagesave(original, 1, ".");
    webpage_t *saved = pageload(1, ".");

    char *URL1 = webpage_getURL(original);
    char *URL2 = webpage_getURL(saved);

    int depth1 = webpage_getDepth(original);
    int depth2 = webpage_getDepth(saved);

    int len1 = webpage_getHTMLlen(original);
    int len2 = webpage_getHTMLlen(saved);

    char *HTML1 = webpage_getHTML(original);
    char *HTML2 = webpage_getHTML(saved);

    eprintf("url diff: %d\n", strcmp(URL1, URL2));
    eprintf("dep diff: %d\n", (depth1 - depth2));
    eprintf("len diff: %d\n", (len1 - len2));
    eprintf("pge diff: %d\n", strcmp(HTML1, HTML2));
    
    webpage_delete(original);
    webpage_delete(saved);
    return 0;
}