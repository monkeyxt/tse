/****************************************************************
 * file   queue.c - queue in c
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 1, 2021
 * 
 * Implementation of a generic queue
 * 
****************************************************************/

#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include"queue.h"

/****************************************************************
 * Define queue data structure
****************************************************************/
typedef struct node {
    struct node *next;
    void *data;
} node_t;

typedef struct queue {
    node_t *front;
    node_t *back;
} q_t;

/****************************************************************
 * Private helper function : check if the queue is empty
****************************************************************/
static bool empty(q_t *qp) {
    return (qp->front == NULL);
}

/****************************************************************
 * Private helper function : create new node
****************************************************************/
static node_t *cnode (void *elementp) {
    node_t *n;
    if(!(n = (node_t*)malloc(sizeof(node_t)))) {
        printf("Error: malloc failed allocating new queue\n");
        return NULL;
    }

    n->data = elementp;
    n->next = NULL;
    return n;
}

/****************************************************************
 * Create an empty queue.
****************************************************************/
queue_t* qopen(void) {
    q_t *n;
    if(!(n = (q_t*)malloc(sizeof(q_t)))) {
        printf("Error: malloc failed allocating new queue\n");
        return NULL;
    }
    n->front = NULL;
    n->back = NULL;
    return (queue_t*)n;
}

/****************************************************************
 * Put element at the end of the queue
 * Return 0 if successful; nonzero otherwise
****************************************************************/
int32_t qput(queue_t *qp, void *elementp) {

    if(qp == NULL || elementp == NULL) return 1;
    node_t *n = cnode(elementp);
    if(n == NULL) return 1;

    // Cast queue_t type to q_t
    q_t *q = (q_t*)qp;

    if(!empty(qp)) {
        q->back->next = n;
        q->back = q->back->next;
        return 0;
    }
    else {
        q->front = n;
        q->back = n;
        return 0;
    }

    return 1;
}

/****************************************************************
 * Get the first first element from queue, 
 * removing it from the queue
****************************************************************/
void* qget(queue_t *qp) {
    
    if(qp == NULL) return NULL;
    q_t *q = (q_t*)qp;
    if(empty(q)) {
        q->back = NULL; // Take care of the back pointer
        return NULL;
    }

    node_t *head = q->front;
    q->front = q->front->next;
    void *data = head->data;
    free(head);

    return data;
}

/****************************************************************
 * Deallocate a queue and free everything in it
****************************************************************/
void qclose(queue_t *qp){

    if(qp == NULL) return;
    q_t *q = (q_t*)qp;
    while(!empty(q)) {
        void* data = qget(q);
        free(data);
    }
    free(q);
}  

/****************************************************************
 * Apply a function to every element of the queue
****************************************************************/
void qapply(queue_t *qp, void (*fn)(void* elementp)) {
    if(qp == NULL || fn == NULL) return;
    q_t *q = (q_t*)qp;
    node_t *pt = q->front;
    while(pt != NULL) {
        fn(pt->data);
        pt = pt->next;
    }
}

/****************************************************************
 * search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
****************************************************************/
void* qsearch(queue_t *qp, 
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp){
    if ((qp == NULL || searchfn == NULL) || skeyp == NULL) return NULL;
    q_t *q = (q_t*)qp;
    node_t *pt = q->front;
    while(pt != NULL) {
        if(searchfn(pt->data, skeyp)) {
            return pt->data;
        }
        pt = pt->next;
    }
    return NULL;
}

/****************************************************************
 * Search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
****************************************************************/
void* qremove(queue_t *qp,
							bool (*searchfn)(void* elementp,const void* keyp),
							const void* skeyp) {

    if ((qp == NULL || searchfn == NULL) || skeyp == NULL) return NULL;

    q_t *q = (q_t*)qp;
    // pt acts as a fast pointer that is always one step ahead of qt
    node_t *pt = q->front;
    node_t *qt = NULL;

    while(pt != NULL) {
        if(searchfn(pt->data, skeyp)) {

            if(pt == q->front && pt == q->back) {
                // If there is only one element in the queue
                q->front = NULL;
                q->back = NULL;
                void *data = pt->data;
                free(pt);
                return data;
            }
            else if(pt == q->front) {
                // If the element is the head of the queue
                q->front = q->front->next;
                void *data = pt->data;
                free(pt);
                return data;
            }
            else if(pt == q->back) {
                // If the element is the back of the queue
                q->back = qt;
                qt->next = NULL;
                void *data = pt->data;
                free(pt);
                return data;
            }
            else {
                // The element is somewhere in the middle
                qt->next = pt->next;
                void *data = pt->data;
                free(pt);
                return data;
            }
        }

        // Advance the pointers
        qt = pt;
        pt = pt->next;
    }

    return NULL;
}

/****************************************************************
 * concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
****************************************************************/
void qconcat(queue_t *q1p, queue_t *q2p) {
    
    q_t *q1 = (q_t*)q1p;
    q_t *q2 = (q_t*)q2p;

    if(q2->front == NULL) {
        free(q2);
    }
    else if (q1->front == NULL && q2->front != NULL) {
        q1->front = q2->front;
        q1->front = q2->front;
        free(q2);
    }
    else {
        q1->back->next = q2->front;
        q1->back = q2->back;
        free(q2);
    }

    return;
}
