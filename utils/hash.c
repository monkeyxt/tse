/****************************************************************
 * file   hash.c - hash in c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 1, 2021
 * 
 * Implementation of a generic hash table
 * 
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include"queue.h"
#include"hash.h"

/****************************************************************
 * Define hash data structure
****************************************************************/
typedef struct hashtable {
	queue_t **buckets;
  	uint32_t size;
} h_t;

/****************************************************************
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
****************************************************************/
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
	uint32_t hash = len, tmp;
	int rem;
	
	if (len <= 0 || data == NULL)
			return 0;
	rem = len & 3;
	len >>= 2;
	/* Main loop */
	for (;len > 0; len--) {
		hash  += get16bits (data);
		tmp    = (get16bits (data+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		data  += 2*sizeof (uint16_t);
		hash  += hash >> 11;
	}
	/* Handle end cases */
	switch (rem) {
	case 3: hash += get16bits (data);
		hash ^= hash << 16;
		hash ^= data[sizeof (uint16_t)] << 18;
		hash += hash >> 11;
		break;
	case 2: hash += get16bits (data);
		hash ^= hash << 11;
		hash += hash >> 17;
		break;
	case 1: hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
	}
	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;
	return hash % tablesize;
}

/****************************************************************
 * hopen -- opens a hash table with initial size hsize
****************************************************************/
hashtable_t *hopen(uint32_t hsize) {
	
	// Allocate hashtable
	h_t *h;
	if(!(h = (h_t*)malloc(sizeof(h_t)))) {
		printf("Error: malloc failed allocating new hashtable\n");
        return NULL;
	}

	// Allocate buckets for hashtable
	if(!(h->buckets = (queue_t **)malloc(sizeof(queue_t *) * hsize))) {
		printf("Error: malloc failed allocating new hashtable buckets\n");
        return NULL;
	}

	// Allocate queues for each of the buckets
	h->size = hsize;
	for(int i = 0; i < h->size; i++) {		
		queue_t *q = qopen();
		if(q == NULL) {
			printf("Error: malloc failed allocating queue\n");
			return NULL;
		}
		(h->buckets)[i] = q;
	}

	return (hashtable_t*)h;

}

/****************************************************************
 * hclose -- closes a hash table
****************************************************************/
void hclose(hashtable_t *htp) {

	if(htp == NULL) return;
	h_t *h = (h_t*)htp;

	// Deallocate queue for each bucket
	for(int i = 0; i < h->size; i++) {
		qclose((h->buckets)[i]);
	}

	// Deallocate buckets and the hashtable
	free(h->buckets);
	free(h);

	return;
}

/****************************************************************
 * hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
****************************************************************/
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {

	if(htp == NULL || ep == NULL || key == NULL) return 1;
	h_t *h = (h_t*)htp;
	uint32_t index = SuperFastHash(key, keylen, h->size);
	queue_t *q = (h->buckets)[index];
	return qput(q, ep);
}

/****************************************************************
 * happly -- applies a function to every entry in hash table 
****************************************************************/
void happly(hashtable_t *htp, void (*fn)(void* ep)) {
	if(htp == NULL || fn == NULL) return;
	h_t *h = (h_t*)htp;
	for(int i = 0; i < h->size; i++) {
		queue_t *q = (h->buckets)[i];
		qapply(q, fn);
	}
}

/****************************************************************
 * hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL 
 * if not found
****************************************************************/
void *hsearch(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen) {
	if(htp == NULL || searchfn == NULL || key == NULL) return NULL;
	h_t *h = (h_t*)htp;		  
	uint32_t index = SuperFastHash(key, keylen, h->size);
	queue_t *q = (h->buckets)[index];
	return qsearch(q, searchfn, key);
}

/****************************************************************
 * hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry 
 * or NULL if not found
****************************************************************/
void *hremove(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen) {
	if(htp == NULL || searchfn == NULL || key == NULL) return NULL;
	h_t *h = (h_t*)htp;
	uint32_t index = SuperFastHash(key, keylen, h->size);
	queue_t *q = (h->buckets)[index];
	return qremove(q, searchfn, key);
}