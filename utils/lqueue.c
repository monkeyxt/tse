/****************************************************************
 * file   lqueue.c - concurrent queue in c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   November 11, 2021
 *
 * Implementation of a concurrent queue with pthread
 *
 ****************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"
#include "lqueue.h"

pthread_mutex_t lock;

lqueue_t *lqopen() {
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);
    queue_t *q = qopen();
    pthread_mutex_unlock(&lock);
    return ((lqueue_t *)q);
}

void lqclose(lqueue_t *qp) {
    pthread_mutex_lock(&lock);
    qclose((queue_t *)qp);
    pthread_mutex_unlock(&lock);
    pthread_mutex_destroy(&lock);
}

int32_t lqput(lqueue_t *qp, void *elementp){
    pthread_mutex_lock(&lock);
    int32_t res = qput((queue_t *)qp, elementp);
    pthread_mutex_unlock(&lock);
    return res;
}

void *lqget(lqueue_t *qp){
    pthread_mutex_lock(&lock);
    void *res = qget(qp);
    pthread_mutex_unlock(&lock);
    return res;
}

void lqapply(lqueue_t *qp, void (*fn)(void *elementp)){
    pthread_mutex_lock(&lock);
    qapply(qp, fn);
    pthread_mutex_unlock(&lock);
}

void* lqsearch(queue_t *qp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp) {

    pthread_mutex_lock(&lock);
    void *res = qsearch(qp, searchfn, skeyp);
    pthread_mutex_unlock(&lock);
    return res;
}

void* lqremove(queue_t *qp,
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp) {
    pthread_mutex_lock(&lock);
    void *res = qremove(qp, searchfn, skeyp);
    pthread_mutex_unlock(&lock);
    return res;
}