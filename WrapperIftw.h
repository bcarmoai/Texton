// WrapperIftw.h: interface for the CWrapperIftw class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRAPPERIFTW_H__B8ECB02B_3EE3_4438_B0EE_3080D1CE1EDC__INCLUDED_)
#define AFX_WRAPPERIFTW_H__B8ECB02B_3EE3_4438_B0EE_3080D1CE1EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/************************************* 
PRIORITY QUEUE CODE BY Georg Kraml
http://www.purists.org/georg/pqueue/
*************************************/

struct somestruct { int prio; int x; int y; };
#define PQDATUM struct somestruct *
#define PQPRIO(p) (p->prio) 

/*
 *  Priority queue structure
 */
struct pqueue 
{
	int size, avail, step;
	PQDATUM *d;
};

/***************************************** 
END OF PRIORITY QUEUE CODE BY Georg Kraml
http://www.purists.org/georg/pqueue/
*****************************************/


/************************************/
/* priority queue code by A. Falcão */
/************************************/

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

/*******************************************/
/* end of priority queue code by A. Falcão */
/*******************************************/

class CWrapperIftw  
{
public:
	void Watershed(int width, int height, int* original, int* peaks, int* costs, 
		int* annotated, int version, CPoint** root_map, CPoint** predec_map); 
	// version argument above:
	// 1 - algorithm 2 of Falcao, Stolfi, Lotufo TR IC-02-012 (calls watershed_fifo_012())
	//     this version is obsolete because it uses queue code that does not have
	//     an arbitrary element removal facility
	// 2 - algorithm 2 of Falcao, Stolfi, Lotufo TR IC-02-012 (calls watershed_buckets())

	CWrapperIftw();
	virtual ~CWrapperIftw();

protected:
	int pqdelete(struct pqueue *q, int dx, int dy);
	int GetNext8Neighbour(int img_width, int img_height, int pointx, int pointy, 
						  int &neighbourx, int &neighboury);
	void watershed_fifo_012(int width, int height, int* original, int* peaks, 
		int* costs, int* annotated, CPoint** root_map); 
	void watershed_buckets(int width, int height, int* original, int* peaks, 
		int* costs, int* annotated, CPoint** root_map, CPoint** predec_map);
	struct pqueue *pqinit(struct pqueue *q, int n);
	int pqinsert(struct pqueue *q, PQDATUM d);
	PQDATUM *pqremove(struct pqueue *q, PQDATUM *d);
	PQDATUM *pqpeek(struct pqueue *q, PQDATUM *d);


/************************************/
/* priority queue code by A. Falcão */
/************************************/

Queue *CreateQueue(int nbuckets, int nelems);
void   DestroyQueue(Queue **Q);
void   ResetQueue(Queue *Q);
int    EmptyQueue(Queue *Q);
void   InsertQueue(Queue *Q, int bucket, int elem);
int    RemoveQueue(Queue *Q);
void   RemoveQueueElem(Queue *Q, int elem, int bucket);
void   UpdateQueue(Queue *Q, int elem, int from, int to);

/*******************************************/
/* end of priority queue code by A. Falcão */
/*******************************************/



};

#endif // !defined(AFX_WRAPPERIFTW_H__B8ECB02B_3EE3_4438_B0EE_3080D1CE1EDC__INCLUDED_)
