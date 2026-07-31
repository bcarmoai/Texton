// SortedList.cpp: implementation of the CSortedList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "SortedList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//  Class CListNode 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListNode::CListNode()
{
	prev = next = NULL;
//	index[0] = index[1] = 0;
	assoData = (void*) NULL;
	sortByData = 0.0;
}

/*
CListNode::CListNode(double data, long index1, long index2)
{
	prev = next = NULL;
	if ( index1 < index2){  // to ensure that index[0] always smaller than index[1]
		index[0] = index1;
		index[1] = index2;
	}	
	else {
		index[0] = index2;
		index[1] = index1;
	}
	sortByData = data;
}
*/
CListNode::CListNode(double sData, void *aData)
{
	prev = next = NULL;

	assoData		= aData;
	sortByData	= sData;
}


CListNode::~CListNode()
{
//	if (assoData != NULL)
//		delete [] assoData;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//  Class CSortedList 

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSortedList::CSortedList()
{
	first = last = curr = NULL;
	nNode = 0;
}

CSortedList::~CSortedList()
{
	while(first != NULL) {
		curr = first;
		first = first->next;
		delete curr;
	}
}


//bool CSortedList::AddNode(CListNode *aNode)
/*
bool	CSortedList::AddNode(double data, long idx1, long idx2)
{
	CListNode *aNode = new CListNode(data, idx1, idx2);
	return InsertNode(aNode);
}
*/

bool	CSortedList::AddNode(double sortByData, void *assoData)
{
	CListNode *aNode = new CListNode(sortByData, assoData);
	return InsertNode(aNode);
}

bool CSortedList::InsertNode(CListNode *aNode)
{
	
	if (nNode == 0) {
		first = last = curr = aNode;
		nNode = 1;
	}

	else {

		// --------------case 1 :  aNode to be inserted after curr ---------------------
		if (curr == NULL)
			curr = first;

		if ( aNode->sortByData > curr->sortByData  )
		{
			do {
				curr = curr->next;
			}while(curr != NULL && aNode->sortByData > curr->sortByData );
			
			if (curr == NULL){
				// add aNode to the end of the list
				last->next = aNode;
				aNode->prev = last;
				aNode->next = NULL;
				last = aNode;
				nNode ++;
				curr = aNode;
			}
			else if (aNode->sortByData <= curr->sortByData )
			{
				// insert aNode before curr (curr cannot equal first in this case)
				curr->prev->next = aNode;
				aNode->prev = curr->prev;
				aNode->next = curr;
				curr->prev = aNode;
				nNode ++;
				curr = aNode;
			}
			else{
				//curr and aNode are equal, no action required
				delete aNode;
			}
		}

		// --------------case 2 :  aNode to be inserted before curr ---------------------

		else //if ( aNode->sortByData <= curr->sortByData)  
		
		{  
			do {
				curr = curr->prev;
			}while(curr != NULL && (aNode->sortByData < curr->sortByData ));

			if ( curr == NULL) {
				// add aNode to the head of the list
				aNode->next = first;
				first->prev = aNode;
				first = aNode;
				nNode ++;
				curr = aNode;
			}
			else //if (aNode->sortByData >= curr->sortByData )
			{
				// insert aNode after curr
				curr->next->prev = aNode;
				aNode->next = curr->next;
				aNode->prev = curr;
				curr->next = aNode;
				nNode ++;
				curr = aNode;
			}
			/*
			else {
				//curr and aNode are equal, no action required
				delete aNode;
			}
*/
		}
		/*
		// --------------case 3 :  curr and aNode equals ----------------------
		else {
				//curr and aNode are equal, no action required
				delete aNode;
		}
	*/
	}

	return true;
}

/*
bool CSortedList::InsertNode(CListNode *aNode)
{
	
	if (nNode == 0) {
		first = last = curr = aNode;
		nNode = 1;
	}

	else {

		// --------------case 1 :  aNode to be inserted after curr ---------------------
		if (curr == NULL)
			curr = first;

		if ( aNode->sortByData > curr->sortByData  
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] > curr->index[0] )
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] == curr->index[0] 
						&& aNode->index[1] > curr->index[1]))
		{
			do {
				curr = curr->next;
			}while(curr != NULL && (aNode->sortByData > curr->sortByData 
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] > curr->index[0] )
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] == curr->index[0] 
						&& aNode->index[1] > curr->index[1])));
			
			if (curr == NULL){
				// add aNode to the end of the list
				last->next = aNode;
				aNode->prev = last;
				aNode->next = NULL;
				last = aNode;
				nNode ++;
				curr = aNode;
			}
			else if (aNode->sortByData < curr->sortByData || aNode->index[0] < curr->index[0] 
						|| aNode->index[1] < curr->index[1])
			{
				// insert aNode before curr (curr cannot equal first in this case)
				curr->prev->next = aNode;
				aNode->prev = curr->prev;
				aNode->next = curr;
				curr->prev = aNode;
				nNode ++;
				curr = aNode;
			}
			else{
				//curr and aNode are equal, no action required
				delete aNode;
			}
		}

		// --------------case 2 :  aNode to be inserted before curr ---------------------

		else if ( aNode->sortByData < curr->sortByData  
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] < curr->index[0] )
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] == curr->index[0] 
						&& aNode->index[1] < curr->index[1]))
		
		{  
			do {
				curr = curr->prev;
			}while(curr != NULL && (aNode->sortByData < curr->sortByData 
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] < curr->index[0] )
				|| ( aNode->sortByData == curr->sortByData && aNode->index[0] == curr->index[0] 
						&& aNode->index[1] < curr->index[1])));

			if ( curr == NULL) {
				// add aNode to the head of the list
				aNode->next = first;
				first->prev = aNode;
				first = aNode;
				nNode ++;
				curr = aNode;
			}
			else if (aNode->sortByData > curr->sortByData || aNode->index[0] > curr->index[0] 
						|| aNode->index[1] > curr->index[1])
			{
				// insert aNode after curr
				curr->next->prev = aNode;
				aNode->next = curr->next;
				aNode->prev = curr;
				curr->next = aNode;
				nNode ++;
				curr = aNode;
			}
			else {
				//curr and aNode are equal, no action required
				delete aNode;
			}

		}
		// --------------case 3 :  curr and aNode equals ----------------------
		else {
				//curr and aNode are equal, no action required
				delete aNode;
		}
	
	}

	return true;
}
*/

CListNode* CSortedList::RemoveNode()
{
	CListNode *firstNode;

	if (first == NULL)
		return first;

	firstNode = first;
	
	if ( nNode == 1){
		first = last = curr = NULL;
		nNode = 0;
	}
	else {
		if ( curr == first)
			curr = first->next;

		first = firstNode->next;
		first->prev = NULL;

		nNode--;
		firstNode->next = NULL;  // firstNode->prev always equals NULL
	}
	
	return firstNode;
}

/*
long CSortedList::SelectNodeWithIndex(long searchIdx, long *neighIdx, long maxNeigh)
{
	long		nFound;
	CListNode	*temp_next;
	nFound = 0;

	this->curr = this->first;
	
	while ( nFound < maxNeigh && curr!=NULL){
		temp_next = curr->next;

		if ( curr->index[0] == searchIdx){
			neighIdx[nFound] = curr->index[1];
			nFound++;
			DeleteCurrNode();
		}
		else if (curr->index[1] == searchIdx){
			neighIdx[nFound] = curr->index[0];
			nFound++;
			DeleteCurrNode();
		}
		
		curr = temp_next;
	}

	if (curr == NULL)
		curr = last;

	return nFound;
}

*/
void CSortedList::DeleteAllNodes()
{
	while(first != NULL) {
		curr = first;
		first = first->next;
		delete curr;
	}

	first = last = curr = NULL;
	nNode = 0;
}

bool CSortedList::MoveNext()
{
	if (curr != NULL) {
		curr = curr->next;
		return true;
	}
	else 
		return false;
}

bool CSortedList::MovePrev()
{
	if (curr != NULL ){ //&& curr->prev != NULL ){
		curr = curr->prev;
		return true;
	}
	else 
		return false;
}	

long CSortedList::MoveNodesFromList(CSortedList *aList)
{
	CListNode		*aNode;
	long				nCopied = 0;

	while ( (aNode = aList->RemoveNode() ) != NULL) {
		this->InsertNode(aNode);
		nCopied ++;
	}

	return nCopied;
}

long CSortedList::GetSize()
{
	return this->nNode;
}

void CSortedList::DeleteCurrNode()
{
	// delete currNode and move currPtr to the next node
	if ( curr == NULL)
		return;

	CListNode *temp_next, *temp_prev;

	temp_next = curr->next;
	temp_prev = curr->prev;

	if ( nNode == 1) {
		delete curr;
		nNode = 0;
		first = last = curr = NULL;
	}
	else if ( curr == first) {
		first = temp_next;
		temp_next->prev = NULL;
		delete curr;
		curr = first;
		nNode--;
	}
	else if ( curr == last) {
		last = temp_prev;
		temp_prev->next = NULL;
		delete curr;
		curr = last;
		nNode--;
	}
	else {
		temp_next->prev = temp_prev;
		temp_prev->next = temp_next;
		delete curr;
		curr = temp_next;
		nNode--;
	}
	return;
}

bool CSortedList::IsEmpty()
{
	return (nNode == 0);
}

/*
bool CSortedList::GetFirstNodeData(double *data, long *idx1, long *idx2)
{
	CListNode *firstNode;

	firstNode = RemoveNode();

	*idx1 = firstNode->index[0];
	*idx2 = firstNode->index[1];
	*data = firstNode->sortByData;

	delete firstNode;

	return true;
}
*/

void CSortedList::MoveToFirst()
{
	curr = first;
}

void CSortedList::MoveToLast()
{
	curr = last;
}

double CSortedList::GetFirstNodeSortedData()
{
	if (first != NULL)
		return first->sortByData;
	else
		return 0;
}

void* CSortedList::GetFirstNodeAssoData()
{
	if (first != NULL)
		return first->assoData;
	else
		return NULL;
}

double CSortedList::GetCurrNodeSortedData()
{
	if (curr != NULL)
		return curr->sortByData;
	else
		return 0;
}

void* CSortedList::GetCurrNodeAssoData()
{
	if (curr != NULL)
		return curr->assoData;
	else
		return NULL;
}

void CSortedList::DeleteFirstNode()
{
	CListNode *firstNode;

	firstNode = RemoveNode();

	delete firstNode;
}
