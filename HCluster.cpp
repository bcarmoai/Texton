// HCluster.cpp: implementation of the HCluster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "HCluster.h"
#include "FlowCluster.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HCluster::HCluster(void* aCluster, int level /* default 0 */, 
									 HCluster *hc1 /* NULL */, HCluster *hc2 /* NULL */, HCluster *parent /*NULL*/)
{
	thisCluster			= aCluster;
	childCluster1		= hc1;
	childCluster2		=	hc2;
	parentCluster		= parent;
	thisLevel				= level;

}


HCluster::HCluster()
{
	thisLevel = 0;
	thisCluster		= NULL;
	childCluster1 = NULL;
	childCluster2 = NULL;
	parentCluster = NULL;
}


HCluster::~HCluster()
{
/* don't delete children as they may 
   have been deleted elsewhere

	if(bChildValid == TRUE)
	{
		if ( childCluster1 != NULL)
			delete childCluster1;
		
		if ( childCluster2 != NULL)
			delete childCluster2;
	}
*/	
	if (thisCluster != NULL)
		delete thisCluster;	

}



//////////////////////////////////////////////////////////
// private functions
//////////////////////////////////////////////////////////

bool HCluster::IsRoot()
{
	return (parentCluster  == NULL);
}


bool HCluster::IsLeaf()
{
	return (childCluster1 != NULL && childCluster2 != NULL);
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//			public 
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


void HCluster::DeleteAllChildClusters()
{
	if ( childCluster1 != NULL) {
			delete childCluster1;
	}

	if ( childCluster2 != NULL) {
			delete childCluster2;
	}

	childCluster1 = childCluster2 = NULL;
	return;

}


void* HCluster::GetCluster()
{
	return thisCluster;
}

int HCluster::GetLevel()
{
	return thisLevel;
}

void HCluster::SetParent(HCluster *parent)
{
	parentCluster = parent;
}

HCluster* HCluster::GetParent()
{
	return parentCluster;
}

HCluster* HCluster::GetFirstChild()
{
	return	childCluster1;
}

HCluster* HCluster::GetSecondChild()
{
	return	childCluster2;
}


void HCluster::SetLevel(int level)
{
	this->thisLevel = level;
}

void HCluster::SetValuesFromFileData(HCluster *child1, HCluster *child2, HCluster *parent, int level)
{
	childCluster1 = child1;
	childCluster2 = child2;
	parentCluster = parent;
	thisLevel = level;
	if(thisCluster!=NULL)
		delete thisCluster;
	thisCluster = new FlowCluster;
}
