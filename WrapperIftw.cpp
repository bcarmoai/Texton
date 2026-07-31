// WrapperIftw.cpp: implementation of the CWrapperIftw class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "WrapperIftw.h"
#include <fstream.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWrapperIftw::CWrapperIftw()
{

}

CWrapperIftw::~CWrapperIftw()
{

}

void CWrapperIftw::Watershed(int width, int height, int * original, int * peaks, 
							 int* costs, int * annotated, int version, CPoint** root_map,
							 CPoint** predec_map)
{
	switch(version)
	{
	case 2:
		watershed_buckets(width, height, original, peaks, costs, annotated, root_map,
			predec_map);
		break;
	default:
		AfxMessageBox("Error: called watershed switcher with unknown algorithm number.");
		break;
	}
	
}

void CWrapperIftw::watershed_buckets(int width, int height, int* original, int* peaks, 
									  int* costs, int* annotated, CPoint** root_map, 
									  CPoint** predec_map)
{
	// implements Algorithm 2 of Falcao, Stolfi & Lotufo 2002 IC-02-012
	// Bernardo Carmo, Southampton Uni., UK

	// C(t), the pixel cost map
	float* c_t = new float[width*height];

	// P(t), the predecessor map
	CPoint* p_t = new CPoint[width*height];

	// R(t), the initial root map
	CPoint* r_t = new CPoint[width*height];

	// create queue
	int qsize = width * height;
	const int no_buckets = 10000;
	int qcount = 0;
	// struct pqueue* pq = pqinit(NULL, qsize);
	Queue* pq = CreateQueue(no_buckets, qsize);

	ofstream of;
	of.open("c:\\iftw_log.txt");

	of << "watershed buckets\n" << flush;

	// fill up queue
	int x, y;
	int bucket_to, elem_to;
	int seedCount = 0;
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			// set P(t) <- NULL
			p_t[width*y+x].x = p_t[width*y+x].y = -1;

			// R(t) <- t
			r_t[width*y+x].x = x;
			r_t[width*y+x].y = y;

			if(peaks[width*y+x]==255)
			{
				// set C(t) <- h(t)
				costs[width*y+x] = 0; //original[width*y+x];   // swap comments for testing

				// insert t in Q
				elem_to = width*y+x;
				bucket_to = costs[width*y+x];
				if(bucket_to < no_buckets)
				{
					InsertQueue(pq, bucket_to, elem_to);
					qcount++;
					seedCount++;
					of << "inserted seed " << seedCount << " (" << x << ", " << y 
					   << ") [c:" << costs[width*y+x] << "][q:" << qcount << "]\n" << flush;
				}
				else
				{
					of	<< "Error: seed pixel (" << x << ", " << y << ") with cost "
						<< costs[width*y+x] << " cannot be inserted in queue with "
						<< no_buckets << " buckets.[q:" << qcount << "]\n" << flush;
					goto QueueExit;
				}
			}
			else
			{
				// set C(t) <- +oo
				costs[width*y+x] = INT_MAX;
			}
		}

	int x_t, y_t; // coordinates of point t, neighbour of s
	int pixel_t;
	int c_prime;
	int pixel_s;
	int x_s, y_s;

	of << "loaded " << seedCount << " seed points\n" << flush;

	while(TRUE)
	{
		// while Q not empty
		// remove pixel s from Q such that f(pi*(s)) is minimum
		pixel_s = RemoveQueue(pq);

		if(pixel_s==NULL)
		{
			of << "no more points to remove\n" << flush;
			break;
		}
		else
		{
			qcount--;
			y_s = (int)floor(((double)pixel_s)/((double)width));
			x_s = pixel_s - y_s * width;
			of << "removed point (" << x_s << ", " << y_s 
			   << ")[c:" << costs[width*y_s+x_s] << "][q:" << qcount << "]\n" << flush;
		}

		// for all neighbours t of s
		int dmyx, dmyy;
		GetNext8Neighbour(0,0,0,0,dmyx,dmyy);
		while(GetNext8Neighbour(width, height, x_s, y_s, x_t, y_t))
		{
			// if C(t) > C(s)
			if(costs[width*y_t+x_t] > costs[width*y_s+x_s])
			{			
				// C' = C(s) + intensity(t)
				c_prime = costs[width*y_s+x_s] + original[width*y_t+x_t];
				
				// if C' < C(t)
				if(c_prime < costs[width*y_t+x_t])
				{
					// if C(t) < inf, delete t from Q
					if(costs[width*y_t+x_t] < INT_MAX)
					{
						
						pixel_t = width*y_t+x_t;
						RemoveQueueElem(pq, pixel_t, costs[width*y_t+x_t]);
						qcount--;

						//if(/* successful delete test */)
						//{
						of << "deleted point (" << x_t << ", " <<  y_t 
						   << ")[c:" << costs[width*y_t+x_t] << "][q:" 
						   << qcount << "]\n" << flush;
						//}
						//else
						//{
						//	of << "Error deleting point (" << x_t << ", " <<  y_t 
						//	   << ")\n" << flush;
						//	goto QueueExit;
						//}
					}
	
					// P(t) <- s
					p_t[width*y_t+x_t].x = x_s;
					p_t[width*y_t+x_t].y = y_s;
					
					// C(t) <- C'
					costs[width*y_t+x_t] = c_prime;
					
					// R(t) <- R(s)
					r_t[width*y_t+x_t] = r_t[width*y_s+x_s];

					// insert t in Q
					elem_to = width*y_t+x_t;
					bucket_to = costs[width*y_t+x_t];
					if(bucket_to < no_buckets)
					{
						InsertQueue(pq, bucket_to, elem_to);
						qcount++;
						of << "inserted point (" << x_t << ", " << y_t 
							<< ")[c:" << costs[width*y_t+x_t] << "][q:" 
							<< qcount << "]\n" << flush;
					}
					else
					{
						of	<< "Error: pixel (" << x_t<< ", " << y_t << ") with cost "
							<< costs[width*y_t+x_t] << " cannot be inserted in queue with "
							<< no_buckets << " buckets.[q:" << qcount << "]\n" << flush;
						goto QueueExit;
					}
						
					// check queue size
					if(qcount > 2000)
					{
						AfxMessageBox("Error: queue too large.");
						of << "Error: queue too large (" << qcount << ")." << flush;
						goto QueueExit;
					}


				}// if(cprime				
			} // if(costs
		}// while(GetNextNeighbour

	}

QueueExit:
	// generate annotated image from pixel labels
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			annotated[width*y+x] = int((r_t[width*y+x].y+r_t[width*y+x].x)/2.0);
		}/*
		else
			annotated[width*y+x] = 255;  // "magic number" for green*/

	DestroyQueue(&pq);

	*predec_map = p_t;
	*root_map = r_t;

}


/************************************* 
 PRIORITY QUEUE CODE BY Georg Kraml
 http://www.purists.org/georg/pqueue/
**************************************/

/*
 *  pqinit: initialize the queue.
 *
 *  Parameters:
 *
 *    q           Pointer to a priority queue, or NULL if the user
 *                wishes to leave it to pqinit to allocate the queue.
 *
 *    n           Numer of queue items for which memory should be
 *                preallocated, that is, the initial size of the
 *                item array the queue uses. If you insert more than
 *                n items to the queue, another n items will
 *                be allocated automatically.
 *
 *  Return values:
 *
 *   non-NULL     Priority queue has been initialized.
 *
 *   NULL         Insufficient memory.
 */
struct pqueue * CWrapperIftw::pqinit(struct pqueue *q, int n)
{
	struct pqueue *tmp = q;

	if (!q && !(q = (struct pqueue*)malloc(sizeof(struct pqueue)))) 
	{
		AfxMessageBox("Memory allocation failed in pqinit.");
		return NULL;
	}
	if (!(q->d = (somestruct**)malloc(sizeof(PQDATUM) * n))) 
	{
		AfxMessageBox("Memory allocation failed in pqinit.");
		if (!tmp) free(q);
		return NULL;
	}
	q->avail = q->step = n;
	q->size = 1;
	return q;
}

/*                  
 *  pqinsert: insert an item into the queue.
 *
 *  Parameters:
 *
 *    q           Pointer to a priority queue.
 *
 *    d           Datum to be inserted.
 *
 *  Return values:
 *
 *    1           The item has been inserted.
 *
 *    0           The item could not be appended. Either the queue i
 *                pointer provided was NULL, or the function was unable 
 *                to allocate the amount of memory needed for 
 *                the new item.
 */
int CWrapperIftw::pqinsert(struct pqueue *q, PQDATUM d)
{
	PQDATUM *tmp;
	int i, newsize;

	if (!q) return 0;
	
	/* allocate more memory if necessary */
	if (q->size >= q->avail) 
	{
		newsize = q->size + q->step;
		if (!(tmp = (somestruct**)realloc(q->d, sizeof(PQDATUM) * newsize))) 
		{
			AfxMessageBox("Memory allocation failed in pqinsert.");
			return 0;
		}
		q->d = tmp;
		q->avail = newsize;		
	}

	/* insert item */
	i = q->size++;
	while (i > 1 && PQPRIO(q->d[i / 2]) < PQPRIO(d)) 
	{
		q->d[i] = q->d[i / 2];
		i /= 2;
	}
	q->d[i] = d;
	return 1;	
} 

/*
 *  pqremove: remove the highest-ranking item from the queue.
 *
 *  Parameters:
 *
 *    p           Pointer to a priority queue.
 *
 *    d           Pointer to the PQDATUM variable that will hold the 
 *                datum corresponding to the queue item removed.               
 *
 *  Return values:
 *
 *    non-NULL    An item has been removed. The variable that d points
 *                to now contains the datum associated with the item
 *                in question.
 *
 *    NULL        No item could be removed. Either the queue pointer
 *                provided was NULL, or the queue was empty. The chunk
 *                of memory that d points to has not been modified.
 */
PQDATUM * CWrapperIftw::pqremove(struct pqueue *q, PQDATUM *d)
{	
	PQDATUM tmp;
	int i = 1, j;

	if (!q || q->size == 1) return NULL;
	*d = q->d[1];
	tmp = q->d[--q->size];
	while (i <= q->size / 2) {
		j = 2 * i;
		if (j < q->size && 
			PQPRIO(q->d[j]) < PQPRIO(q->d[j + 1])) {
			j++;
		}
		if (PQPRIO(q->d[j]) <= PQPRIO(tmp)) {
			break;
		}
		q->d[i] = q->d[j];
		i = j;
	}
	q->d[i] = tmp;
	return d;	
} 

/*
 *  pqpeek: access highest-ranking item without removing it.
 *
 *  Parameters:
 *
 *    q           Pointer to a priority queue.
 *
 *    d           Pointer to the PQDATUM variable that will hold the
 *                datum corresponding to the highest-ranking item.
 *                
 *  Return values:
 *
 *    non-NULL   Success. The variable that d points to now contains
 *               the datum associated with the highest-ranking item.
 *
 *    NULL       Failure. Either the queue pointer provided was NULL,
 *               or the queue was empty. The chunk of memory that d
 *               points to has not been modified.
 */
PQDATUM * CWrapperIftw::pqpeek(struct pqueue *q, PQDATUM *d)
{
	if (!q || q->size == 1) return NULL;
	*d = q->d[1];
	return d;
}

/****************************************** 
 END OF PRIORITY QUEUE CODE BY Georg Kraml
*******************************************/

// delete a specific member of the queue
// BSC 9/10/03
int CWrapperIftw::pqdelete(struct pqueue *q, int dx, int dy)
{
	PQDATUM tmp;
	int i, j;

	if (!q || q->size == 1)
		return FALSE;

	BOOL bFound = FALSE;
	// exhaustive search for pointer
	for(i=1; i<q->size; i++)
	{
		if(q->d[i]->x == dx &&
		   q->d[i]->y == dy)
		{
			bFound = TRUE;
			break;
		}
	}

	// return if coordinates not found
	if(bFound == FALSE)
		return FALSE;

	tmp = q->d[--q->size];
	while (i <= q->size / 2) {
		j = 2 * i;
		if (j < q->size && 
			PQPRIO(q->d[j]) < PQPRIO(q->d[j + 1])) {
			j++;
		}
		if (PQPRIO(q->d[j]) <= PQPRIO(tmp)) {
			break;
		}
		q->d[i] = q->d[j];
		i = j;
	}
	q->d[i] = tmp;
	return TRUE;	

} 

int CWrapperIftw::GetNext8Neighbour(int img_width, int img_height, int pointx, int pointy, 
									int &neighbourx, int &neighboury)
{
	static int neigh_no = 0;
	static BOOL ripple = FALSE;

	if(img_width == 0 && img_height == 0)
	{
		neigh_no = 0;
		ripple = FALSE;
		return 0;
	}

	if(ripple == TRUE)
		return 0;

Restart:
	switch(neigh_no)
	{
		// 0 degrees
	case 0:
		if(pointx+1>=img_width)
		{
			neigh_no = 90;
			goto Restart;
		}
		neighbourx = pointx + 1;
		neighboury = pointy;
		neigh_no = 45;
		return 1;
		// 45 degrees
	case 45:
		if(pointx+1>=img_width || pointy-1<0)
		{
			neigh_no = 90;
			goto Restart;
		}
		neighbourx = pointx + 1;
		neighboury = pointy - 1;
		neigh_no = 90;
		return 1;

		// 90 degrees
	case 90:
		if(pointy-1<0)
		{
			neigh_no = 180;
			goto Restart;
		}
		neighbourx = pointx;
		neighboury = pointy - 1;
		neigh_no = 135;
		return 1;

		// 135 degrees
	case 135:
		if(pointx-1<0 || pointy-1<=0)
		{
			neigh_no = 180;
			goto Restart;
		}
		neighbourx = pointx - 1;
		neighboury = pointy - 1;
		neigh_no = 180;
		return 1;

		// 180 degrees
	case 180:
		if(pointx-1<0)
		{
			neigh_no = -90;
			goto Restart;
		}
		neighbourx = pointx - 1;
		neighboury = pointy;
		neigh_no = -135;
		return 1;

		// -135 degrees
	case -135:
		if(pointx-1<0 || pointy+1>=img_height)
		{
			neigh_no = -90;
			goto Restart;
		}
		neighbourx = pointx - 1;
		neighboury = pointy + 1;
		neigh_no = -90;
		return 1;

		// -90 degrees
	case -90:
		if(pointy+1>=img_height)
		{
			neigh_no = -45;
			goto Restart;
		}
		neighbourx = pointx;
		neighboury = pointy + 1;
		neigh_no = -45;
		return 1;

		// -45 degrees
	case -45:
		if(pointx+1>=img_width || pointy+1>=img_height)
		{
			neigh_no = 0;
			ripple = TRUE;
			return 0;
		}
		neighbourx = pointx + 1;
		neighboury = pointy + 1;
		neigh_no = 0;
		ripple = TRUE;
		return 1;

	default:
		AfxMessageBox("Unexpected error in neighbour switch case.");
		return 0;

	}

}

/************************************/
/* priority queue code by A. Falcão */
/************************************/


/* A priority queue Q consists of two data structures: a circular queue C 
and a table L that encodes all possible doubly-linked lists.

   Q requires that the maximum cost of an edge in the graph be a non-negative 
   integer less than the number of buckets in C.
   Q->C.first[i] gives the first element that is in bucket i.
   Q->C.last[i]  gives the last  element that is in bucket i.
   Q->C.nbuckets gives the number of buckets in C.
   Q->current gives the current bucket being processed.

   All possible doubly-linked lists are represented in L. Each bucket contains
   a doubly-linked list that is treated as a FIFO.
   Q->L.elem[i].next: the next element to i 
   Q->L.elem[i].prev: the previous element to i
   Q->L.nelems: gives the total number of elements that can be inserted in Q 
   (It is usually the number of pixels in a given image or the number of nodes in a graph)

   Insertions and deletions are done in O(1).
   Updates may take O(n), where n is the number of buckets.
*/

Queue* CWrapperIftw::CreateQueue(int nbuckets, int nelems)
{
  Queue *Q=NULL;

  Q = (Queue *) malloc(1*sizeof(Queue));
  
  if (Q != NULL) {
    Q->C.first = (int *)malloc(nbuckets * sizeof(int));
    Q->C.last  = (int *)malloc(nbuckets * sizeof(int));
    Q->C.nbuckets = nbuckets;
    if ( (Q->C.first != NULL) && (Q->C.last != NULL) ){
      Q->L.elem = (Node *)malloc(nelems*sizeof(Node));
      Q->L.nelems = nelems;
      if (Q->L.elem != NULL){
	ResetQueue(Q);
      } else
	AfxMessageBox("Error A in CreateQueue");	
    } else
      AfxMessageBox("Error B in CreateQueue");
  } else 
    AfxMessageBox("Error C in CreateQueue");

  return(Q);
}

void CWrapperIftw::ResetQueue(Queue *Q)
{
  int i;

  Q->C.current = 0;
  
  for (i=0; i < Q->C.nbuckets; i++)
    Q->C.first[i]=Q->C.last[i]=NULL;
	
  for (i=0; i < Q->L.nelems; i++){
    Q->L.elem[i].next =  Q->L.elem[i].prev = NULL;
  }

}

void CWrapperIftw::DestroyQueue(Queue **Q)
{
  Queue *aux;

  aux = *Q;
  if (aux != NULL) {
    if (aux->C.first != NULL) free(aux->C.first);
    if (aux->C.last  != NULL) free(aux->C.last);
    if (aux->L.elem  != NULL) free(aux->L.elem);
    free(aux);
    *Q = NULL;
  }
}

void CWrapperIftw::InsertQueue(Queue *Q, int bucket, int elem)
{
  if (Q->C.first[bucket] == NULL){ 
    Q->C.first[bucket]   = elem;  
    Q->L.elem[elem].prev = NULL;
  }else {
    Q->L.elem[Q->C.last[bucket]].next = elem;
    Q->L.elem[elem].prev = Q->C.last[bucket];
  }
  
  Q->C.last[bucket]     = elem;
  Q->L.elem[elem].next  = NULL;
}

int CWrapperIftw::RemoveQueue(Queue *Q)
{
  int elem=NULL, next;
  int last;

  /** moves to next element or returns EMPTY queue **/
  if (Q->C.first[Q->C.current] == NULL) {
    last = Q->C.current;
    
    Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
    
    while ((Q->C.first[Q->C.current] == NULL) && (Q->C.current != last)) {
      Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
    }
    
    if (Q->C.first[Q->C.current] == NULL)
      return NULL;
  }

  elem = Q->C.first[Q->C.current];
  
  next = Q->L.elem[elem].next;
  if (next == NULL) {         /* there is a single element in the list */
    Q->C.first[Q->C.current] = Q->C.last[Q->C.current]  = NULL;    
  }
  else {
    Q->C.first[Q->C.current] = next;
    Q->L.elem[next].prev = NULL;
  }

  return elem;
}

void CWrapperIftw::RemoveQueueElem(Queue *Q, int elem, int bucket)
{
  int prev,next;

  prev = Q->L.elem[elem].prev;
  next = Q->L.elem[elem].next;
  
  /* if elem is the first element */
  if (Q->C.first[bucket] == elem) {
    Q->C.first[bucket] = next;
    if (next == NULL) /* elem is also the last one */
      Q->C.last[bucket] = NULL;
    else
      Q->L.elem[next].prev = NULL;
  }
  else{   /* elem is in the middle or it is the last */
    Q->L.elem[prev].next = next;
    if (next == NULL) /* if it is the last */
      Q->C.last[bucket] = prev;
    else 
      Q->L.elem[next].prev = prev;
  }
}

void CWrapperIftw::UpdateQueue(Queue *Q, int elem, int from, int to)
{
  RemoveQueueElem(Q, elem, from);
  InsertQueue(Q, to, elem);
}

int CWrapperIftw::EmptyQueue(Queue *Q)
{
  int last;
  if (Q->C.first[Q->C.current] != NULL)
    return 0;
  
  last = Q->C.current;
  
  Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets);
  
  while ((Q->C.first[Q->C.current] == NULL) && (Q->C.current != last)) {
    Q->C.current = (Q->C.current + 1) % (Q->C.nbuckets); 
  }
  
  return (Q->C.first[Q->C.current] == NULL);
}


/*******************************************/
/* end of priority queue code by A. Falcão */
/*******************************************/


