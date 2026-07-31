// SortedList.h: interface for the CSortedList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SORTEDLIST_H__E29883E1_27B4_4ECC_98EC_EFBF37784414__INCLUDED_)
#define AFX_SORTEDLIST_H__E29883E1_27B4_4ECC_98EC_EFBF37784414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

class CListNode
{
friend class CSortedList;

public:

private:
	CListNode*	prev;
	CListNode*	next;
	CListNode*	curr;
//	float				mergeError;
	double			sortByData;
	void				*assoData;
//	long				index[2];

private:
	CListNode();
//	CListNode(double data, long index1, long index2);
	CListNode(double sData, void *aData);
	virtual ~CListNode();
};


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

class CSortedList  
{
public:
	void DeleteFirstNode();
	void MoveToLast();
	void MoveToFirst();
//	bool			AddNode(double data, long idx1, long idx2);
	bool			AddNode(double sortByData, void *assoData);
//	bool			GetFirstNodeData(double *data, long *idx1, long *idx2);
	
	double		GetFirstNodeSortedData();
	void * 		GetFirstNodeAssoData();
	double		GetCurrNodeSortedData();
	void * 		GetCurrNodeAssoData();

	bool			IsEmpty();
	long			GetSize();
	long			MoveNodesFromList(CSortedList *aList);
	bool			MovePrev();
	bool			MoveNext();
	void			DeleteAllNodes();
	void			DeleteCurrNode();
//	long			SelectNodeWithIndex(long searchIdx, long *neighIdx, long nNeigh);

	CSortedList();
	virtual ~CSortedList();

private:
	CListNode*	RemoveNode();
	bool				InsertNode(CListNode *aNode);
	CListNode*		first;
	CListNode*		last;
	CListNode*		curr;
	long					nNode;
};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#endif // !defined(AFX_SORTEDLIST_H__E29883E1_27B4_4ECC_98EC_EFBF37784414__INCLUDED_)
