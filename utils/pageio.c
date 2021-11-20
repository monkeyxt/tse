/****************************************************************
 * file   pageio.c 
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 1, 2021
 * 
 * Saves and loads webpages
 * 
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include"pageio.h"
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>


/****************************************************************
 * Define macro, globals and helper functions
****************************************************************/
#define eprintf(format, ...) do {                 \
    if (verbose)                                  \
        fprintf(stderr, format, ##__VA_ARGS__);   \
} while(0)
#define verbose 0

struct stat st = {0};
const int url_len = 128;


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
 * Pageload - loads a html page to local directory
 * \param id        The assigned id of the page
 * \param dirname   The directory to store the page in
 * 
 * \return          page if sucess and NULL if otherwise
****************************************************************/
webpage_t *pageload(int id, char *dirnm) {
    
    // Populate filename
    char *filename = malloc(sizeof(char)*strlen(dirnm) + sizeof(char) * 16);
    sprintf(filename, "%s/%d", dirnm, id);

    if(access(filename, R_OK) == 0) {
        // If file is accessable
        FILE *inputf = fopen(filename, "r");
        int depth, len;
        char url[url_len];

        // Read file
        fscanf(inputf, "%s\n%d\n%d\n", url, &depth, &len);
        char *html = malloc(sizeof(char)*(len + 1));

        for(int i = 0; i < len; i++) {
            char buffer = fgetc(inputf);
            if(buffer == EOF) break;
            html[i] = buffer;
        }
        html[len] = '\0';
        
        // Cleanup
        webpage_t *newpage = webpage_new(url, depth, html);
        free(filename);
        fclose(inputf);
        return newpage;
    }

    // Cleanup
    free(filename);
    return NULL;
}