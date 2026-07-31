#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "common.h"

#define QSIZE    65536 

typedef struct _node { 
  int next;  /* next node */
  int prev;  /* prev node */
} Node;

typedef struct _doublylinkedlists {
  Node *elem;  /* all possible doubly-linked lists of the circular queue */
  int nelems;  /* total number of elements */
} DoublyLinkedLists; 

typedef struct _circularqueue { 
  int *first;  /* list of the first elements of each doubly-linked list */
  int *last;   /* list of the last  elements of each doubly-linked list  */
  int nbuckets;  /* number of buckets in the circular queue */
  int current;   /* current bucket */
} CircularQueue;

typedef struct _queue { /* Priority queue by Dial implemented as
                           proposed by A. Falcao */
  CircularQueue C;
  DoublyLinkedLists L;
} Queue;

Queue *CreateQueue(int nbuckets, int nelems);
void   DestroyQueue(Queue **Q);
void   ResetQueue(Queue *Q);
int    EmptyQueue(Queue *Q);
void   InsertQueue(Queue *Q, int bucket, int elem);
int    RemoveQueue(Queue *Q);
void   RemoveQueueElem(Queue *Q, int elem, int bucket);
void   UpdateQueue(Queue *Q, int elem, int from, int to);

#endif







