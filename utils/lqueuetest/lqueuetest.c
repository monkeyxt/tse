/****************************************************************
 * file  lqueuetest.c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Tests if the lqueue.h module is working as intended
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
#include"hash.h"
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

typedef struct object {
    char c[256];
    int x; int y;
} object_t;


/****************************************************************
 * Dynamically allocate the objects on the heap
****************************************************************/
object_t *cnode(const char *c, int x, int y) {
    
    object_t *n;
    if(!(n = (object_t*)malloc(sizeof(object_t)))) {
        printf("Error: malloc failed allocating object");
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
void *thread1(void *queue){
    lqueue_t *q = (lqueue_t*)queue;
    object_t *o1 = cnode("o1", 1, 10);
    object_t *o2 = cnode("o2", 2, 20);

    // Test 1: place nodes into the queue
    int rv1 = lqput(q, (void*)o1);
    int rv2 = lqput(q, (void*)o2);
    if(rv1 || rv2) {
        printf("Error: lqueue insert failed\n");
        exit(EXIT_FAILURE);
    }

    // Test 2: search for an existing target and a non-existant
    // target. The second target should return a NULL element.
    char *target1 = "o1";
    char *target2 = "dummy";
    object_t *res1 = lqsearch(q, searchfn, (void*)target1);
    object_t *res2 = lqsearch(q, searchfn, (void*)target2);
    pobject(res1); pobject(res2);

    // Test 3: test the lapply function on an non-empty queue
    lqapply(q, pobject);

    // Test 4: removing all elements created by this thread
    char *target3 = "o2";
    object_t *res3 = lqremove(q, searchfn, (void*)target1);
    object_t *res4 = lqremove(q, searchfn, (void*)target3);

    free(res3);
    free(res4);

    return NULL;
}


/****************************************************************
 * Thread2
****************************************************************/
void *thread2(void *queue){
    lqueue_t *q = (lqueue_t*)queue;
    object_t *o1 = cnode("o3", 3, 30);
    object_t *o2 = cnode("o4", 4, 40);

    int rv1 = lqput(q, (void*)o1);
    int rv2 = lqput(q, (void*)o2);
    if(rv1 || rv2) {
        printf("Error: lqueue insert failed\n");
        exit(EXIT_FAILURE);
    }

    char *target1 = "o3";
    char *target2 = "dummy";
    object_t *res1 = lqsearch(q, searchfn, (void*)target1);
    object_t *res2 = lqsearch(q, searchfn, (void*)target2);
    pobject(res1); pobject(res2);

    char *target3 = "o4";
    object_t *res3 = lqremove(q, searchfn, (void*)target1);
    object_t *res4 = lqremove(q, searchfn, (void*)target3);

    free(res3);
    free(res4);

    return NULL;
}


/****************************************************************
 * Thread3
****************************************************************/
void *thread3(void *queue){
    lqueue_t *q = (lqueue_t*)queue;
    object_t *o1 = cnode("o5", 5, 50);
    object_t *o2 = cnode("o6", 6, 60);

    int rv1 = lqput(q, (void*)o1);
    int rv2 = lqput(q, (void*)o2);
    if(rv1 || rv2) {
        printf("Error: lqueue insert failed\n");
        exit(EXIT_FAILURE);
    }

    char *target1 = "o5";
    char *target2 = "dummy";
    object_t *res1 = lqsearch(q, searchfn, (void*)target1);
    object_t *res2 = lqsearch(q, searchfn, (void*)target2);
    pobject(res1); pobject(res2);

    char *target3 = "o6";
    object_t *res3 = lqremove(q, searchfn, (void*)target1);
    object_t *res4 = lqremove(q, searchfn, (void*)target3);

    free(res3);
    free(res4);

    return NULL;
}


/****************************************************************
 * Tests if lqueue works for multiple threads
****************************************************************/
int main() {

    lqueue_t *q = lqopen();
    pthread_t p1, p2, p3;

    int res1 = pthread_create(&p1, NULL, thread1, q);
    int res2 = pthread_create(&p2, NULL, thread2, q);
    int res3 = pthread_create(&p3, NULL, thread3, q);

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
    printf("Info: final queue should be empty\n");
    
    // Test 5: test the lapply on an empty queue.
    lqapply(q, pobject);
    lqclose(q);

    exit(EXIT_SUCCESS);      
}