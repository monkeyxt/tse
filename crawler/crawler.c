/****************************************************************
 * file   crawler.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 16, 2021          
 * 
 * Implementation of a concurrent crawerl in c.
 * Last updated: November 13, 2021
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<pthread.h>
#include<unistd.h>
#include<errno.h>
#include"webpage.h"
#include"lhash.h"
#include"lqueue.h"


/****************************************************************
 * Define macro, globals and helper functions
****************************************************************/
#define eprintf(format, ...) do {                 \
    if (verbose)                                  \
        fprintf(stderr, format, ##__VA_ARGS__);   \
} while(0)
#define verbose 0

#define __MAXB 50
#define __MAXCHAR 128
struct stat st = {0};

// Define global queue and hashtable
lqueue_t *u_queue;
lhashtable_t *vis;
int id = 0;

// Define pthread args
typedef struct args {
    char seedURL[__MAXCHAR];
    char pagedir[__MAXCHAR];
    int maxdepth;
} args_t;


/****************************************************************
 * Search function for hashtable to match url string
****************************************************************/
bool searchfn(void *p, const void *s) {
    char *p_url = (char*)p ;
    char *s_url = (char*)s;
    return !strcmp(p_url, s_url);
}


/****************************************************************
 * Private helper function: Validate that the input string is a
 * valid unsigned 32 bit integer.
 * \param s     The string to be converted
 * \return      1 if the string is a u_int32 and 0 otherwise    
****************************************************************/
static bool valid_uint(const char *s) {
    long long x;
    char *pEnd;
    x = strtoul(s, &pEnd, 10);
    return s != pEnd && !(*pEnd) && x >= 0 && x < UINT32_MAX;
}


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
 * Pagesave - saves a html page to local directory
 * \param pagep     The page to be saved
 * \param id        The assigned id of the page
 * \param dirname   The directory to store the page in
 * 
 * \return          0 if sucess and non-zero if otherwise
****************************************************************/
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
        eprintf("Error: create page file failed for %s\n", filename);
        eprintf("Error %d \n", errno);
        return -1;
    }
    fprintf(outputf, "%s\n%d\n%d\n%s\n", url, depth, len, html);
    fclose(outputf);

    // Deallocate the filename string
    free(filename);
    return 0;
}


/****************************************************************
 * cpage - checks validity of URL and creates a webpage
 * \param h         The indexing hashtable 
 * \param depth     The depth of the page to be created
 * \param url       The URL of the page to be created
 * 
 * \return          0 if sucess and non-zero if otherwise
****************************************************************/
webpage_t *cpage(lhashtable_t *h, int depth, char *url) {
    if(!(NormalizeURL(url) && IsInternalURL(url))) {
        eprintf("Panic: bad link %s\n", url);
        return NULL;
    }
    if(lhsearch(h, &searchfn, url, strlen(url)) != NULL) {
        eprintf("Panic: duplicate URL %s\n", url);
        return NULL;
    }
    
    webpage_t *page = webpage_new(url, depth + 1, NULL);
    if(page == NULL) {
        eprintf("Failed to create page: %s", url);
        return NULL;
    }

    return page;
}


/****************************************************************
 * Crawler - starts a BFS of a designated URL
 * \param seedURL   The starting URL for crawling 
 * \param pagedir   The directory to store the crawled pages in
 * \param maxdepth  The maximum depth for crawling
 * 
 * \return          0 if sucess and non-zero if otherwise
****************************************************************/
void *crawler(void *input) {

    args_t *info = (args_t*)input;

    // Start BFS of the current seed webpage
    int depth = 0;
    webpage_t *p;
    while((p = (webpage_t*)lqget(u_queue)) != NULL) {
        
        depth = webpage_getDepth(p);        // The depth of the current page
        int pos = 0;                        // Position of the crawling cursor
        char *url = NULL;                   // New pointer to the fectched URL

        if(webpage_fetch(p)) {
            
            // Save the current html page
            if(pagesave(p, ++id, info->pagedir) == -1) {
                eprintf("Error: failed to save page %s\n", webpage_getURL(p));
                return NULL;
            }

            printf("Level %d -- Scanning %s\n", depth, webpage_getURL(p));
            if(depth < info->maxdepth) {
                while((pos = webpage_getNextURL(p, pos, &url)) > 0) {
                    
                    printf("Info: found URL %s\n", url);
                    webpage_t *newpage = cpage(vis, depth, url);
                    if(newpage != NULL) {
                        lhput(vis, url, url, strlen(url));
                        lqput(u_queue, newpage);
                    }
                    else {
                        free(url);
                    }
                }
            }
        }
        else {
            eprintf("Failed to create page: %s", url);
        }

        // Pop the element
        webpage_delete(p);
    }

    return NULL;
}



/****************************************************************
 * checkinput - checks the cmd input provided by the user
 * \return error code:
 *              0: parsing success
 *              1: wrong number of arguments
 *              3. non uint_32 integer depth
 *              4. negative depth value 
 *              5. non uint_32 thread number
 *              6. negative thread number
****************************************************************/
int checkinput(int argc, char* argv[]) {
    
    // Parse the cmdline inputs
    if(argc != 5) {
        printf("usage: crawler <seedurl> <pagedir> <maxdepth> <threadnum>\n");
        return 1;
    }

    // Make sure that directory is valid
    char *dirname = argv[2];
    if(stat(dirname, &st) == -1) mkdir(dirname, 0777);
    if(access(dirname, W_OK)) chmod(dirname, W_OK);

    // Sanity check, make sure maxdepth is an integer
    int maxdepth = -1;
    if(!valid_uint(argv[3])) {
        printf("Error: invalid maximum depth\n");
        return 3;
    }

    // Make sure depth is an non-negative integer
    maxdepth = convert_uint(argv[3]);
    if(maxdepth < 0) {
        printf("Error: maxdepth must be an non-negative integer\n");
        return 4;
    }

    // Sanity check, make sure thread number is an
    int threadnum = -1;
    if(!valid_uint(argv[4])) {
        printf("Error: invalid thread number\n");
        return 5;
    }

    // Make sure depth is an non-negative integer
    threadnum = convert_uint(argv[4]);
    if(threadnum < 0) {
        printf("Error: threadnum must be an non-negative integer\n");
        return 6;
    }

    return 0;
}


/****************************************************************
 * Crawler - starts a BFS of a designated URL
 * usage: crawler <seedurl> <pagedir> <maxdepth>
****************************************************************/
int main(int argc, char *argv[]) {

    int error = checkinput(argc, argv);
    if(error != 0) {
        eprintf("Error parsing arguments: %d\n", error);
        exit(EXIT_FAILURE);
    }

    int maxdepth = convert_uint(argv[3]);
    int threadnum = convert_uint(argv[4]);

    // Initialize queue for list of URLs
    u_queue = lqopen();
    if(u_queue == NULL) {
        printf("Error: Failed to initialize queue\n");
        return -1;
    }

    // Initalize hashtable to store visited URLs. The hashtable assumes a
    // maximum breadth as defined by the global parameter maxbreadth.
    vis = lhopen((maxdepth + 1) * __MAXB);
    if(vis == NULL) {
        printf("Error: Failed to initalize hashtable\n");
        return -1;
    }

    // Fetch the seed page. Exit failure if the seed page is invalid or if 
    // creating local page failed.
    webpage_t *seed = webpage_new(argv[1], 0, NULL);
    if(seed == NULL || !IsInternalURL(webpage_getURL(seed))) {
        eprintf("Error: Failed to fetch seed page %s\n", argv[1]);
        return -1;
    }
    lqput(u_queue, seed);
    char* seedcopy = (char*)calloc(strlen(argv[1]) + 1, sizeof(char));
    strcpy(seedcopy, argv[1]);
    lhput(vis, seedcopy, seedcopy, strlen(seedcopy));

    // Create thread pool and call the crawler function
    printf("Crawler working with %s and depth %d...\n", argv[1], maxdepth);
    args_t *args = (args_t*)malloc(sizeof(args_t));
    strcpy(args->seedURL, argv[1]);
    strcpy(args->pagedir, argv[2]);
    args->maxdepth = maxdepth;
    
    pthread_t threads[threadnum];
    for(int i = 0; i < threadnum; i++) {
        if(pthread_create(&threads[i], NULL, crawler, (void*)args) != 0) {
            eprintf("Error: thread %d create failed\n", i);
            exit(EXIT_FAILURE);
        }
        else {
            eprintf("Info: thread %d create success\n", i);
            sleep(2); // Allow time for the first thread to crawl some pages
        }
    }

    // Destroy threads
    for(int i = 0; i < threadnum; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            eprintf("Error: destroying thread %d failed\n", i);
            exit(EXIT_FAILURE);
        }
        else{
            eprintf("Info: destroying thread %d success\n", i);
        }
    }

    // Cleanup
    free(args);

    // Close lqueue and lhash
    lqclose(u_queue);
    lhclose(vis);
    exit(EXIT_SUCCESS);
}
