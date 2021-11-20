/****************************************************************
 * file   lhash.c - concurrent hashtable
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   Novemver 11, 2021
 * 
 * Implementation of a concurrent hashtable
 * 
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<pthread.h>
#include"hash.h"
#include"lhash.h"

pthread_mutex_t hashlock;

lhashtable_t *lhopen(uint32_t hsize) {
    pthread_mutex_init(&hashlock, NULL);
    pthread_mutex_lock(&hashlock);
    lhashtable_t*h = (lhashtable_t*)hopen(hsize);
    pthread_mutex_unlock(&hashlock);
    return h;
}

void lhclose(lhashtable_t *htp){
    pthread_mutex_lock(&hashlock);
    hclose((hashtable_t*)htp);
    pthread_mutex_unlock(&hashlock);
    pthread_mutex_destroy(&hashlock);
    return;
}

int32_t lhput(lhashtable_t *htp, void *ep, const char *key, int keylen){
    pthread_mutex_lock(&hashlock);
    int32_t res = hput((hashtable_t*)htp, ep, key, keylen);
    pthread_mutex_unlock(&hashlock);
    return res;
}

void lhapply(lhashtable_t *htp, void (*fn)(void* ep)){
    pthread_mutex_lock(&hashlock);
    happly((hashtable_t*)htp, fn);
    pthread_mutex_unlock(&hashlock);
    return;
}

void *lhsearch(lhashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen) {
    pthread_mutex_lock(&hashlock);
    void *res = hsearch((hashtable_t*)htp, searchfn, key, keylen);
    pthread_mutex_unlock(&hashlock);
    return (res);
}

void *lhremove(lhashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen) {
    pthread_mutex_lock(&hashlock);
    void *res = hremove((hashtable_t*)htp, searchfn, key, keylen);
    pthread_mutex_unlock(&hashlock);
    return (res);         
}
