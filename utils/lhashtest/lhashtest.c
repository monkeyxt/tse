/****************************************************************
 * file  lhashtest.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Tests if the lhash.h module is working as intended
 * 
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>
#include"lhash.h"
#include"lqueue.h"


/****************************************************************
 * Define macro, globals and helper functions
****************************************************************/
#define eprintf(format, ...) do {                 \
    if (verbose)                                  \
        fprintf(stderr, format, ##__VA_ARGS__);   \
} while(0)
#define verbose 1

#define __HSIZE__ 1000
#define __MAXCHAR__ 256

typedef struct object {
    char c[__MAXCHAR__];
    int x; int y;
} object_t;


/****************************************************************
 * Dynamically allocate the objects on the heap
****************************************************************/
object_t *cnode(const char *c, int x, int y) {
    
    object_t *n;
    if(!(n = (object_t*)malloc(sizeof(object_t)))) {
        printf("Error: malloc failed allocating object\n");
        return NULL;
    }
    strcpy(n->c, c);
    n->x = x; n->y = y;
    return n;
}


/****************************************************************
 * A search function that matches char values
****************************************************************/
bool searchfn(void *elementp, const void*keyp) {
    object_t *o1 = (object_t*)elementp;
    char *x = (char*)keyp;
    return !strcmp(o1->c, x);
}


/****************************************************************
 * A print function that dumps object data
****************************************************************/
void pobject(void *element) {
    object_t *o = (object_t*)element;
    if(o != NULL) {
        printf("char: %s, int1: %d, int2: %d\n", 
                o->c, o->x, o->y);
        return;
    }
    printf("Error: NULL element\n");
    return;
}


/****************************************************************
 * Thread1
****************************************************************/
void *thread1(void *hash){
    lhashtable_t *q = (lhashtable_t*)hash;
    object_t *o1 = cnode("o1", 1, 40);
    object_t *o2 = cnode("o2", 2, 30);

    // Test 1: put elements into the councurrent hashtable
    lhput(q, (void*)o1, (char*)(o1->c), strlen((char*)o1->c));
    lhput(q, (void*)o2, (char*)(o2->c), strlen((char*)o2->c));

    // Test 2: search elements from the hashtable. The first 
    // target should return object_t o1. The second search
    // should return a NULL object. 
    char *target1 = "o1";
    char *target2 = "dummy";
    object_t *res1 = lhsearch(q, searchfn, (void*)target1, 
                        strlen((void*)target1));
    object_t *res2 = lhsearch(q, searchfn, (void*)target2, 
                        strlen((void*)target2));
    pobject(res1); pobject(res2);


    // Test 3: test lhapply on a non-empty hashtable
    printf("Thread1: testing lhapply on non-empty hashtable\n");
    lhapply(q, pobject);

    // Test 4: test removing all elements created by this thread.
    char *target3 = "o2";
    object_t *res3 = lhremove(q, searchfn, (void*)target1, 
                        strlen((void*)target1));
    object_t *res4 = lhremove(q, searchfn, (void*)target3, 
                        strlen((void*)target3));

    free(res3);
    free(res4);

    return NULL;
}


/****************************************************************
 * Thread2
****************************************************************/
void *thread2(void *hash){
    lhashtable_t *q = (lhashtable_t*)hash;
    object_t *o1 = cnode("o3", 3, 40);
    object_t *o2 = cnode("o4", 4, 30);

    lhput(q, (void*)o1, (char*)(o1->c), strlen((char*)o1->c));
    lhput(q, (void*)o2, (char*)(o2->c), strlen((char*)o2->c));

    char *target1 = "o3";
    char *target2 = "dummy";
    object_t *res1 = lhsearch(q, searchfn, (void*)target1, 
                        strlen((void*)target1));
    object_t *res2 = lhsearch(q, searchfn, (void*)target2, 
                        strlen((void*)target2));
    pobject(res1); pobject(res2);

    char *target3 = "o4";
    object_t *res3 = lhremove(q, searchfn, (void*)target1, 
                        strlen((void*)target1));
    object_t *res4 = lhremove(q, searchfn, (void*)target3, 
                        strlen((void*)target3));
    free(res3);
    free(res4);

    return NULL;
}


/****************************************************************
 * Thread3
****************************************************************/
void *thread3(void *hash){
    lhashtable_t *q = (lhashtable_t*)hash;
    object_t *o1 = cnode("o5", 5, 40);
    object_t *o2 = cnode("o6", 6, 30);

    lhput(q, (void*)o1, (char*)(o1->c), strlen((char*)o1->c));
    lhput(q, (void*)o2, (char*)(o2->c), strlen((char*)o2->c));

    char *target1 = "o5";
    char *target2 = "dummy";
    object_t *res1 = lhsearch(q, searchfn, (void*)target1, 
                        strlen((void*)target1));
    object_t *res2 = lhsearch(q, searchfn, (void*)target2, 
                        strlen((void*)target2));
    pobject(res1); pobject(res2);

    char *target3 = "o6";
    object_t *res3 = lhremove(q, searchfn, (void*)target1, 
                        strlen((void*)target1));
    object_t *res4 = lhremove(q, searchfn, (void*)target3, 
                        strlen((void*)target3));
    free(res3);
    free(res4);

    return NULL;
}


/****************************************************************
 * Tests if lhash works for multiple threads
****************************************************************/
int main() {

    lhashtable_t *h = lhopen(__HSIZE__);
    pthread_t p1, p2, p3;

    int res1 = pthread_create(&p1, NULL, thread1, h);
    int res2 = pthread_create(&p2, NULL, thread2, h);
    int res3 = pthread_create(&p3, NULL, thread3, h);

    if(res1 || res2 || res3) {
        printf("Fatal: thread creation failed\n");
        exit(EXIT_FAILURE);
    }

    sleep(3);

    int res4 = pthread_join(p1, NULL);
    int res5 = pthread_join(p2, NULL);
    int res6 = pthread_join(p3, NULL);

    if(res4 || res5 || res6) {
        printf("Fatal: thread failed\n");
        exit(EXIT_FAILURE);
    }

    sleep(3);
    printf("Info: final hash should be empty\n");

    // Test 6: test lhappy on empty hashtable
    lhapply(h, pobject);
    lhclose(h);

    exit(EXIT_SUCCESS);      
}
