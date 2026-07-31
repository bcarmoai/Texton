// FlowAbstraction.cpp: implementation of the FlowAbstraction class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "FlowAbstraction.h"
#include <fstream.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef ANSI
#define ANSI
#endif
#include "nrutil.h"

typedef struct {
	FlowCluster	mergeCluster;
	int					childIndex1;
	int					childIndex2;
} MergeNode;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FlowAbstraction::FlowAbstraction()
{
	m_initialised = false;

	m_vecSizeX			= 0; 
	m_vecSizeY			= 0;
	m_vecSize				= 0;
	
	m_oriVec					= NULL;
	m_rootCluster			= NULL;
	m_allCluster			= NULL;
	m_leafCluster			= NULL;
	m_nonLeafCluster	= NULL;

	m_nCluster				= 0;
	m_nNonLeafCluster	= 0;
	m_nLeafCluster		= 0;

	m_ptsMap3D = NULL;
}


FlowAbstraction::~FlowAbstraction()
{
	// ClearClusters();

	if ( m_oriVec  != NULL)
	{
		delete [] m_oriVec;
		m_oriVec = NULL;
	}			

	if ( m_allCluster != NULL)
	{
		for(int i = 0; i < m_nCluster; i++)
			delete m_allCluster[i];

		delete [] m_allCluster;
	}

	if( m_ptsMap3D != NULL)
		delete [] m_ptsMap3D;

}



////////////////////////////////////////////////////////////////////
//									ClearClusters()
////////////////////////////////////////////////////////////////////

void FlowAbstraction::ClearClusters()
{
	if (m_rootCluster != NULL) {
		delete m_rootCluster;
		m_rootCluster			= NULL;
	}	
	
	return;
}



////////////////////////////////////////////////////////////////////
//									Initialise()
////////////////////////////////////////////////////////////////////

BOOL FlowAbstraction::Initialise(CVector3D *vec, int sizeX, int sizeY, int sizeZ)
{
	if (m_initialised)
		return false;

	int		idx = 0;
	m_vecSizeX = sizeX;
	m_vecSizeY = sizeY;
	m_vecSizeZ = sizeZ;
	m_vecSize	 = sizeX * sizeY * sizeZ;
	m_zeroSkipSize = 0;
	for (idx = 0; idx < m_vecSize; idx ++) 
		if(vec[idx].Norm()>0.0)
			m_zeroSkipSize++;

	m_nLeafCluster = m_zeroSkipSize;
	m_nNonLeafCluster = m_nLeafCluster - 1;
	m_nCluster = m_nLeafCluster + m_nNonLeafCluster;
	
	m_allCluster = new HCluster*[m_nCluster];
	m_leafCluster = m_allCluster;
	m_nonLeafCluster = m_allCluster + m_nLeafCluster;

	m_oriVec = new CVector3D[m_vecSize];
	m_ptsMap3D = new int[m_vecSize];
	
	for (idx = 0; idx < m_vecSize; idx ++) 
		m_oriVec[idx] = vec[idx];

	m_initialised = true;

	return true;
}


////////////////////////////////////////////////////////////////////
//									StartClustering()
////////////////////////////////////////////////////////////////////

bool FlowAbstraction::StartClustering()
{
	// return true if compression is successful
	// return false if failed

	if ( !m_initialised) {
		AfxMessageBox("ERROR : FlowAbstraction not initialised.");
		return false;
	}

	if ( m_rootCluster != NULL){
		AfxMessageBox("ERROR : FlowAbstraction already contains clusters.");
		return false;
	}
	
	/////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////
/*
#ifdef _DEBUG
			CMemoryState memTracker1;
			memTracker1.Checkpoint();
			CString sms233;
#endif 
*/

	const int		MAX_NEIGHS = 6;
	int				*neighCount = new int[m_nCluster];
	int				**neighIdxs = new int*[m_nLeafCluster];

	int				idx = 0;
	int				x, y, z;
	int				i;
	int				rightNeigh, bottomNeigh, backNeigh;
	int				sizeX, sizeY, sizeZ;
	
	MergeNode		*mergeNode;

	// init neighCount
	for(i = 0; i < m_nCluster; i++) 
		neighCount[i] = 0;
	
	ofstream of;
	of.open("c:\\nNode.txt");

	//////////////////////////////////////
	// create leaf clusters
	of << "Creating leaf clusters... \n" << flush;
	CreateLeafClusters();
	
	testCluster = (FlowCluster*)m_allCluster[0]->GetCluster();

/*
#ifdef _DEBUG
			CMemoryState memTracker2;
			memTracker2.Checkpoint();
			CMemoryState memDiff;
			memDiff.Difference(memTracker1, memTracker2);
			sms233.Format("StartClustering A\n");
			TRACE(sms233);
			memDiff.DumpStatistics();
#endif 
*/


	//////////////////////////////////////
	// initialise neighbourIdx 
	
	sizeX = m_vecSizeX ;
	sizeY = m_vecSizeY ;
	sizeZ = m_vecSizeZ ;

	int ni;
	for (i = 0; i < m_nLeafCluster; i++)
	{
		neighIdxs[i] = new int[MAX_NEIGHS];  // max 6 neighbours 
		for(ni=0; ni<MAX_NEIGHS; ni++)
			neighIdxs[i][ni]=0;

	}

	// set up temporary cluster position map for neighbour lookup
	int* posMap = new int[m_vecSize];

	for(i = 0; i < m_vecSize; i++)
		posMap[i]=NO_CLUSTER;

	CPoint3D centr;
	for(i = 0; i < m_nLeafCluster; i++)
	{
		centr = ((FlowCluster*)m_allCluster[i]->GetCluster())->GeoCentroid();
		posMap[centr.z*sizeX*sizeY + centr.y*sizeX + centr.x]=i;
	}

	// right neighbour index
	for (y = 0; y < sizeY; y++) 
		for(z = 0; z < sizeZ; z++)
			for (x = 0; x < sizeX-1; x++)
			{
				idx = z*sizeX*sizeY + y*sizeX + x;
				rightNeigh = idx + 1;

				if(posMap[idx]==NO_CLUSTER || posMap[rightNeigh]==NO_CLUSTER)
					continue;
				
				neighIdxs[posMap[idx]][neighCount[posMap[idx]]] = posMap[rightNeigh];
				neighIdxs[posMap[rightNeigh]][neighCount[posMap[rightNeigh]]] = posMap[idx];

				neighCount[posMap[idx]] ++;
				neighCount[posMap[rightNeigh]] ++;
			}

	// bottom neighbour index
	for (y = 0; y < sizeY-1; y++) 
		for(z = 0; z < sizeZ; z++)
			for (x = 0; x < sizeX; x++)
			{
				idx = z*sizeX*sizeY + y*sizeX + x;
				bottomNeigh = idx + sizeX;
				
				if(posMap[idx]==NO_CLUSTER || posMap[bottomNeigh]==NO_CLUSTER)
					continue;

				neighIdxs[posMap[idx]][neighCount[posMap[idx]]] = posMap[bottomNeigh];
				neighIdxs[posMap[bottomNeigh]][neighCount[posMap[bottomNeigh]]] = posMap[idx];

				neighCount[posMap[idx]] ++;
				neighCount[posMap[bottomNeigh]] ++;
			}

	// back neighbour index
	for (y = 0; y < sizeY; y++) 
		for(z = 0; z < sizeZ-1; z++)
			for (x = 0; x < sizeX-1; x++)
			{
				idx = z*sizeX*sizeY + y*sizeX + x;
				backNeigh = idx + sizeX*sizeY;

				if(posMap[idx]==NO_CLUSTER || posMap[backNeigh]==NO_CLUSTER)
					continue;

				neighIdxs[posMap[idx]][neighCount[posMap[idx]]] = posMap[backNeigh];
				neighIdxs[posMap[backNeigh]][neighCount[posMap[backNeigh]]] = posMap[idx];

				neighCount[posMap[idx]] ++;
				neighCount[posMap[backNeigh]] ++;
			}

	delete [] posMap;

	////////////////////////////////////////////////
	////////////////////////////////////////////////

	CSortedList		*errorList;
	CSortedList		*neighList;

	int				cIndex1;
	int				cIndex2;
	int				nNeigh, nNeigh1, nNeigh2;
	int				*neighIdx, *neighIdx1, *neighIdx2;
	int				nFound1, nFound2;
	int				cIndex;
	int				currLevel = 0;
	FlowCluster * mergedCluster;
		
	cIndex = m_nLeafCluster;
	currLevel = 1;
	neighList = new CSortedList;

/*
#ifdef _DEBUG
			CMemoryState memTracker3;
			memTracker3.Checkpoint();
			sms233.Format("StartClustering B\n");
			TRACE(sms233);
#endif 
*/

	// initialise error list
	of << "Creating errorlist... \n" << flush;
	errorList = CreateErrorList(neighCount, neighIdxs);
	
/*
#ifdef _DEBUG
			CMemoryState memTracker4;
			memTracker4.Checkpoint();
			sms233.Format("StartClustering C\n");
			TRACE(sms233);
			memDiff.Difference(memTracker3, memTracker4);
			memDiff.DumpStatistics();
			TRACE("");
#endif 
*/

	for ( i = 0; i < m_nLeafCluster; i++)
		delete [] neighIdxs[i];
	delete [] neighIdxs;

	bnnode = FALSE;
	while ( (!errorList->IsEmpty())) 
	{

		//
		// step 1:
		// find neighbours of the two clusters in firstNode
		//

		of << errorList->GetSize() << "\n" << flush;

		mergeNode = (MergeNode*) errorList->GetFirstNodeAssoData();
		cIndex1 = mergeNode->childIndex1;
		cIndex2 = mergeNode->childIndex2;

		errorList->DeleteFirstNode();

		//  create mergedCluster
		mergedCluster = new FlowCluster(mergeNode->mergeCluster);
		m_allCluster[cIndex] = new HCluster(mergedCluster, currLevel, m_allCluster[cIndex1], m_allCluster[cIndex2]);
	
		m_allCluster[cIndex1]->SetParent(m_allCluster[cIndex]);
		m_allCluster[cIndex2]->SetParent(m_allCluster[cIndex]);
		
		delete mergeNode;
		mergeNode = NULL;

		// MemSave 27/1/04
		// ((FlowCluster*)m_allCluster[cIndex1]->GetCluster())->MemSaveDeletePoints();
		// ((FlowCluster*)m_allCluster[cIndex2]->GetCluster())->MemSaveDeletePoints();

		// generate new neigbourList

		nNeigh1 = neighCount[cIndex1];
		nNeigh2 = neighCount[cIndex2];

		nNeigh1 --;   // one neighbour (firstNode) has already been extracted
		nNeigh2 --;   
		
		ASSERT ( nNeigh1 >= 0 && nNeigh2 >= 0);
		
		nNeigh = nNeigh1 + nNeigh2;

		if (nNeigh> 0) 
		{

			//////////////////////////////////////////////
			//  update neighCount[] of the new cluster

			//if(errorList->GetSize() < 900)
			//	MessageBeep(0);

			neighIdx = new int[nNeigh];
			neighIdx1 = neighIdx;
			neighIdx2 = neighIdx + nNeigh1 ;
			nFound1 = nFound2 = 0;

			if (nNeigh1 > 0)
				nFound1 = SearchListForIndex(errorList, cIndex1, neighIdx1, nNeigh1);
				
			if (nNeigh2 > 0)
				nFound2 = SearchListForIndex(errorList, cIndex2, neighIdx2, nNeigh2);
			
			///////////////////////
			//		error checking 
			
			ASSERT (nFound1 == nNeigh1 && nFound2 == nNeigh2);
			// BSC presumably because the children are neighbours
			// of the parent and vice-versa

			nNeigh = EliminateDuplicateIndex(neighIdx, nNeigh1, nNeigh2, neighCount);
			neighCount[cIndex]	= nNeigh;

			// Cal error of merging Cluster[cIndex] and its neighbours
			CalNeighError(cIndex, neighIdx, nNeigh, neighList);
			/// Found to allocate a lot of memory 12-1-04

			// merge neighList to errorList
			UpdateErrorList(errorList, neighList, cIndex);

			delete [] neighIdx;
			cIndex++;
			currLevel++;

		}  // end if total nNeigh > 0
	}

/*
#ifdef _DEBUG
			CMemoryState memTracker5;
			memTracker5.Checkpoint();
			memDiff.Difference(memTracker4, memTracker5);
			sms233.Format("StartClustering D\n");
			TRACE(sms233);
			memDiff.DumpStatistics();
			TRACE("End of StartClustering");
#endif 
*/

	delete errorList;
	delete neighList;

	delete [] neighCount;

	m_rootCluster = m_allCluster[m_nCluster - 1];

	of.close();

	///////////////////////////////////////////
	//   error checking 

	//ASSERT(m_rootCluster->GetLevel() == m_nNonLeafCluster);
	//FlowCluster *fc;
	//fc = (FlowCluster*)m_rootCluster->GetCluster();
	//ASSERT(fc->GetNPts() == m_vecSize);
	//ASSERT(fc->GetNSamplePts() == 0);

	return true;
}




////////////////////////////////////////////////////////////////////
//									CreateLeafCluster()
////////////////////////////////////////////////////////////////////

int FlowAbstraction::CreateLeafClusters()
{
	int			x, y, z;
	int			idx, index;
	FlowCluster	*fCluster;
	const int	nPts = 1;
	int			nSamplePts;

	int			sIdx, sx, sy, sz;
	int			sx_lo, sx_hi;
	int			sy_lo, sy_hi;
	int			sz_lo, sz_hi;

	CPoint3D	*coor;
	CVector3D	*vec;
	CPoint3D	*sampleCoor;
	CVector3D	*sampleVec;

	idx = 0;

	for(z=0; z < m_vecSizeZ; z++) 
	{
		
		if (z ==0)		sz_lo = 0;
		else	sz_lo = z - 1;
		if (z == (m_vecSizeZ - 1))		sz_hi = z;
		else	sz_hi = z + 1;
		
		
		for(y=0; y < m_vecSizeY; y++) 
		{
			
			if (y ==0)		sy_lo = 0;
			else	sy_lo = y - 1;
			if (y == (m_vecSizeY - 1))		sy_hi = y;
			else	sy_hi = y + 1;
			
			for(x = 0; x < m_vecSizeX; x++) 
			{
				
				if (x ==0)		sx_lo = 0;
				else	sx_lo = x - 1;
				if (x == (m_vecSizeX - 1))		sx_hi = x;
				else	sx_hi = x + 1;

				if(!(m_oriVec[z*m_vecSizeX*m_vecSizeY + y*m_vecSizeX + x].Norm()>0.0))
					continue;
				
				nSamplePts = 0;
				for ( sz = sz_lo; sz <= sz_hi; sz++) 
					for ( sy = sy_lo; sy <= sy_hi; sy++) 
						for ( sx = sx_lo; sx <= sx_hi; sx++)
							if(m_oriVec[sz*m_vecSizeX*m_vecSizeY + sy*m_vecSizeX + sx].Norm()>0.0
								&& !( sy == y && sx == x && sz == z ))
								nSamplePts++;
				
				sampleCoor = new CPoint3D[nSamplePts];
				sampleVec  = new CVector3D[nSamplePts];
				coor = new CPoint3D[nPts];
				vec = new CVector3D[nPts];
				
				sIdx = 0;
				index = z*m_vecSizeX*m_vecSizeY + y*m_vecSizeX + x;
				
				for ( sz = sz_lo; sz <= sz_hi; sz++) 
					for ( sy = sy_lo; sy <= sy_hi; sy++) 
						for ( sx = sx_lo; sx <= sx_hi; sx++)
							if ( sy == y && sx == x && sz == z) 
							{
								coor[0] = CPoint3D(x, y, z);
								vec[0] = m_oriVec[index];
							}
							else 
							{
								if(m_oriVec[sz*m_vecSizeX*m_vecSizeY + sy*m_vecSizeX + sx].Norm()>0.0)
								{
									sampleVec[sIdx] = 
										m_oriVec[sz*m_vecSizeX*m_vecSizeY + sy * m_vecSizeX + sx];
									sampleCoor[sIdx] = CPoint3D(sx, sy, sz);
									sIdx++;
								}
							}
				
				///////////////////////////////////////////////////////////
				
				fCluster = new FlowCluster;
				if ( !fCluster->InitialiseCluster(coor, vec, nPts, 
					sampleCoor, sampleVec, nSamplePts, (m_vecSizeZ-1)>1?TRUE:FALSE))
				{
					AfxMessageBox("FlowCluster initialisation failed");
					exit(-1);
				}

				fCluster->MemSaveDeletePoints();

				m_leafCluster[idx]= new HCluster((void*) fCluster);
				idx++;
				
			}
		}
	}

	return 1;
}




////////////////////////////////////////////////////////////////////
//										CreateErrorList()									
////////////////////////////////////////////////////////////////////

CSortedList* FlowAbstraction::CreateErrorList(int	*neighCount, int **neighIdxs)
{
	int						idx, nidx;
	int						neigh;
	MergeNode			*mergeNode;
	CSortedList		*errorList;
	double				uvwError;
	double				oldError, childError1;
	FlowCluster		*child1, *child2;

	ofstream of;
	of.open("c:\\errorList.txt");

	errorList = new CSortedList;

	of << "Error List started. m_nLeafCluster=" << m_nLeafCluster << "\n";
	of << "Processing neighbour :\n";

	for ( idx = 0; idx < m_nLeafCluster; idx++)
	{
		of << idx << "\n" << flush;
	
		child1 = (FlowCluster*) m_allCluster[idx]->GetCluster();
		// childSecondMoment1 = child1->GetUvwSecondMoment();
		childError1 = child1->GetUvwError();

/*
#ifdef _DEBUG
		CString sms144;
		sms144.Format("neighCount %i\n", neighCount[idx]);
		TRACE(sms144);
#endif 
*/			
		for ( nidx = 0; nidx < neighCount[idx]; nidx++) 
		{
			if ( (neigh = neighIdxs[idx][nidx]) > idx) 
			{
				
				child2 = (FlowCluster*) m_allCluster[neigh]->GetCluster();
				// oldSecondMoment = child2->GetUvwSecondMoment() + childSecondMoment1;
				oldError = child2->GetUvwError() + childError1;

/*
#ifdef _DEBUG
				CMemoryState memTracker1;
				memTracker1.Checkpoint();
				CString sms233;
#endif 
*/

				mergeNode = new MergeNode;
/*
#ifdef _DEBUG
				CMemoryState memTracker2;
				memTracker2.Checkpoint();
				CMemoryState memDiff;
				memDiff.Difference(memTracker1, memTracker2);
				sms233.Format("A %i\n", errorList->GetSize());
				TRACE(sms233);
				memDiff.DumpStatistics();
#endif
*/
				MergeClusters(idx, neigh,
					/*child1, child2,*/ &(mergeNode->mergeCluster)); 
/*			
#ifdef _DEBUG
				CMemoryState memTracker3;
				memTracker3.Checkpoint();
				memDiff.Difference(memTracker2, memTracker3);
				sms233.Format("B %i\n", errorList->GetSize());
				TRACE(sms233);
				memDiff.DumpStatistics();
#endif 
*/
				mergeNode->childIndex1 = idx;
				mergeNode->childIndex2 = neigh;
				// totalSecondMoment = mergeNode->mergeCluster.GetUvwSecondMoment();
				/*
				uError = mergeNode->mergeCluster.GetuError();
				vError = mergeNode->mergeCluster.GetvError();
				wError = mergeNode->mergeCluster.GetwError();
				*/
				uvwError = mergeNode->mergeCluster.GetUvwError();
				
				//  modified on 7Aug 03
				
				//errorList->AddNode( totalSecondMoment -	oldSecondMoment, (void*) mergeNode);
				errorList->AddNode((uvwError)- oldError, (void*) mergeNode);
			}
		}
	}

	of.close();

	return errorList;
}




////////////////////////////////////////////////////////////////////
//									MergeClusters
////////////////////////////////////////////////////////////////////


void FlowAbstraction::MergeClusters(int fc1, int fc2, FlowCluster* fc)
{
/*
#ifdef _DEBUG
			CMemoryState memTracker1;
			memTracker1.Checkpoint();
			CString sms233;
#endif
*/

	CPoint3D	*ptsInCluster1;
	CPoint3D	*ptsInCluster2;
	CPoint3D	*samplePtsInCluster1;
	CPoint3D	*samplePtsInCluster2;
	
	int			nPts1, nPts2, nPts;
	int			nSamplePts1, nSamplePts2, nSamplePts;
	
	CPoint3D	*pts;
	CPoint3D	*samplePts;
	CRect3D		boundRect, boundRect1, boundRect2;

	int			i, idx;
	int			x, y, z;

	samplePts = samplePtsInCluster1 = samplePtsInCluster2 = NULL;
	pts = ptsInCluster1 = ptsInCluster2 = NULL;

	// compute bounding rect of all sample pts
	boundRect1 = ((FlowCluster*)m_allCluster[fc1]->GetCluster())->BoundRect();
	boundRect2 = ((FlowCluster*)m_allCluster[fc2]->GetCluster())->BoundRect();
	boundRect.UnionRect(boundRect1, boundRect2);	
	boundRect.InflateRect(1, 1, 1);
	boundRect.IntersectRect(boundRect, CRect3D(0, 0, m_vecSizeX, m_vecSizeY, 0, m_vecSizeZ));
	
	for ( z = boundRect.GetFront(); z < boundRect.GetBack(); z++) 
		for ( y = boundRect.GetTop(); y < boundRect.GetBottom(); y++) 
			for ( x = boundRect.GetLeft(); x < boundRect.GetRight(); x++) 
				m_ptsMap3D[z*m_vecSizeY*m_vecSizeX+y*m_vecSizeX+x] = 0;

	// get children's sample points
				  // first call to get number of points
	nSamplePts1 = nSamplePts2 = 0;
	MemSaveGetSamplePoints(m_allCluster[fc1], NULL, nSamplePts1);
	MemSaveGetSamplePoints(m_allCluster[fc2], NULL, nSamplePts2);
	nSamplePts = nSamplePts1 + nSamplePts2;
	
	if ( nSamplePts1 > 0) 
	{
				  // second call to get points
		samplePtsInCluster1 = new CPoint3D[nSamplePts1];
		nSamplePts1 = 0;
		MemSaveGetSamplePoints(m_allCluster[fc1], samplePtsInCluster1, nSamplePts1);
	}
	else{ AfxMessageBox("Error A: sample points < 0"); }

	if ( nSamplePts2 > 0) 
	{
				  // second call to get points
		samplePtsInCluster2 = new CPoint3D[nSamplePts2];
		nSamplePts2 = 0;
		MemSaveGetSamplePoints(m_allCluster[fc2], samplePtsInCluster2, nSamplePts2);
	}
	else{ AfxMessageBox("Error B: sample points < 0"); }

	// allocate concatenated sample points array
	if ( nSamplePts > 0) 
	{
		samplePts = new CPoint3D[nSamplePts];
	}
	else{ AfxMessageBox("Error C: sample points < 0"); }

	nPts1 = nPts2 = 0;
	MemSaveGetClusterPoints(m_allCluster[fc1], NULL, nPts1);
	MemSaveGetClusterPoints(m_allCluster[fc2], NULL, nPts2);
	nPts	= nPts1 + nPts2;
	ptsInCluster1 = new CPoint3D[nPts1];
	ptsInCluster2 = new CPoint3D[nPts2];
	nPts1 = nPts2 = 0;
	MemSaveGetClusterPoints(m_allCluster[fc1], ptsInCluster1, nPts1);
	MemSaveGetClusterPoints(m_allCluster[fc2], ptsInCluster2, nPts2);

	// allocate concatenated cluster points, cluster vectors and sample vectors
	pts = new CPoint3D[nPts];
	CVector3D* vec = new CVector3D[nPts];
	CVector3D* sampleVec = new CVector3D[nSamplePts];

	////////////////////////////////////////////////

	CPoint3D pt;
	/*
#ifdef _DEBUG
			CMemoryState memTracker2;
			memTracker2.Checkpoint();
			CMemoryState memDiff;
			memDiff.Difference(memTracker1, memTracker2);
			sms233.Format("MergeClusters A %i\n", nSamplePts);
			TRACE(sms233);
			memDiff.DumpStatistics();
#endif 
	*/

	// m_ptsMap3D content: 
	// 0-empty; 1-sample point; 2-cluster point
	// to avoid the same point being added repeatedly to samples list
	for ( i = 0; i < nSamplePts1; i++)
	{
		pt = samplePtsInCluster1[i];
		m_ptsMap3D[pt.z*m_vecSizeY*m_vecSizeX+pt.y*m_vecSizeX+pt.x] = 1;
	}

	for ( i = 0; i < nSamplePts2; i++)
	{
		pt = samplePtsInCluster2[i];
		m_ptsMap3D[pt.z*m_vecSizeY*m_vecSizeX+pt.y*m_vecSizeX+pt.x] = 1;
	}

	idx = 0;
	for ( i = 0; i < nPts1; i++)
	{
		pts[idx] = ptsInCluster1[i];
		m_ptsMap3D[pts[idx].z*m_vecSizeY*m_vecSizeX+pts[idx].y*m_vecSizeX+pts[idx].x] = 2;
		vec[idx] = m_oriVec[pts[idx].z*m_vecSizeY*m_vecSizeX+pts[idx].y*m_vecSizeX+pts[idx].x];
		idx++;
	}
	for ( i = 0; i < nPts2; i++)
	{
		pts[idx] = ptsInCluster2[i];
		m_ptsMap3D[pts[idx].z*m_vecSizeY*m_vecSizeX+pts[idx].y*m_vecSizeX+pts[idx].x] = 2;
		vec[idx] = m_oriVec[pts[idx].z*m_vecSizeY*m_vecSizeX+pts[idx].y*m_vecSizeX+pts[idx].x];
		idx++;
	}

	////////////////////////////////////////////////
	if (nSamplePts > 0)
	{
		idx = 0;
		for ( z = boundRect.GetFront(); z < boundRect.GetBack(); z++) 
			for ( y = boundRect.GetTop(); y < boundRect.GetBottom(); y++) 
				for ( x = boundRect.GetLeft(); x < boundRect.GetRight(); x++) 
					if ( m_ptsMap3D[z*m_vecSizeY*m_vecSizeX+y*m_vecSizeX+x] == 1)
					{
						samplePts[idx] = CPoint3D(x, y, z);
						sampleVec[idx] = m_oriVec[z*m_vecSizeY*m_vecSizeX+y*m_vecSizeX+x];
						idx++;
					}

		nSamplePts = idx;
	}

	////////////////////////////////////////////////#
/*
#ifdef _DEBUG
			CMemoryState memTracker3;
			memTracker3.Checkpoint();
			memDiff.Difference(memTracker2, memTracker3);
			sms233.Format("MergeClusters B\n");
			TRACE(sms233);
			memDiff.DumpStatistics();
#endif 
*/
	// initialise a flowCluster with the merged cluster of points
	fc->InitialiseCluster(pts, vec, nPts, samplePts, sampleVec, nSamplePts, (m_vecSizeZ-1)>1?TRUE:FALSE);
	fc->MemSaveDeletePoints();

/*
#ifdef _DEBUG
			CMemoryState memTracker4;
			memTracker4.Checkpoint();
			memDiff.Difference(memTracker3, memTracker4);
			sms233.Format("MergeClusters C\n");
			TRACE(sms233);
			memDiff.DumpStatistics();
#endif 
*/

	if ( samplePtsInCluster1 != NULL)
		delete [] samplePtsInCluster1;
	if ( samplePtsInCluster2 != NULL)
		delete [] samplePtsInCluster2;

	if ( samplePts != NULL)
		delete [] samplePts;
	if ( sampleVec != NULL)
		delete [] sampleVec;

	delete [] ptsInCluster1;
	delete [] ptsInCluster2;

	delete [] pts;
	delete [] vec;

/*
#ifdef _DEBUG
			CMemoryState memTracker5;
			memTracker5.Checkpoint();
			memDiff.Difference(memTracker4, memTracker5);
			sms233.Format("MergeClusters D\n");
			TRACE(sms233);
			memDiff.DumpStatistics();
#endif 
*/

}



////////////////////////////////////////////////////////////////////
//									EliminateDuplicateIndex
////////////////////////////////////////////////////////////////////

int FlowAbstraction::EliminateDuplicateIndex(int *index, int n1, int n2, int *neighCount)
{
	// return the no of valid element in the array, with duplicated ones set to -1
	int	total;
	int	i, j;

	if (n1 == 0 || n2 == 0) {
		total = n1 + n2;
		return (total);
	}


	int	nDuplicate;
	int	*index1, *newIndex2;
	int	*tempIndex2 = new int[n2];
	int	idx;

	idx	= 0;
	total = n1 + n2;
	index1 = index;
	newIndex2 = index + n1;
	nDuplicate = 0;

	for (i = 0; i < n2; i++) 
		tempIndex2[i] = newIndex2[i];

	for (i = 0; i < n2; i++) {
		for ( j = 0; j < n1; j++) {
			if ( tempIndex2[i] == index1[j]){
				neighCount[index1[j]] --;
				nDuplicate++;
				break;
			}  
		}
		if ( j == n1) {
			index[n1+idx] = tempIndex2[i];
			idx++;
		}
	}

	// error checking
	ASSERT ( (n1+idx) == (total-nDuplicate));
	
	delete [] tempIndex2;
	
	return (total - nDuplicate);

}



////////////////////////////////////////////////////////////////////
//									SearchListForIndex()
////////////////////////////////////////////////////////////////////

int FlowAbstraction::SearchListForIndex(CSortedList *errorList, int searchIdx, int *neighIdx, int nNeigh)
{
	int		nFound;

	if (nNeigh <= 0)
		return 0;

	MergeNode	*data;

	errorList->MoveToFirst();
	nFound = 0;
	
	while (nFound < nNeigh  && (data = (MergeNode*)errorList->GetCurrNodeAssoData()) != NULL) {

		if ( data->childIndex1 == searchIdx){
			neighIdx[nFound] = data->childIndex2;
			nFound++;
			errorList->DeleteCurrNode();
			delete data;
		}
		else if (data->childIndex2 == searchIdx){
			neighIdx[nFound] = data->childIndex1;
			nFound++;
			errorList->DeleteCurrNode();
			delete data;
		}
		else 
			errorList->MoveNext();

	}

	return nFound;		
}


////////////////////////////////////////////////////////////////////
//									CalNeighError()
////////////////////////////////////////////////////////////////////

void FlowAbstraction::CalNeighError(int cIndex, int *neighIdx, int nNeigh, 
									CSortedList *neighList)
{
	int					i;
	MergeNode		*mergeNode;
	double			uvwError;
	double			oldError, childError1;

	FlowCluster	*child1, *child2;

	child1 = (FlowCluster*) m_allCluster[cIndex]->GetCluster();
	// childSecondMoment1 = child1->GetUvwSecondMoment();
	childError1 = child1->GetUvwError();
			
	for(i=0; i<nNeigh; i++){
		
		child2 = (FlowCluster*) m_allCluster[neighIdx[i]]->GetCluster();
		// oldSecondMoment = child2->GetUvwSecondMoment() + childSecondMoment1;
		oldError = child2->GetUvwError() + childError1;


//		if(bnnode)
//		AfxMessageBox("B");
		mergeNode = new MergeNode;
		MergeClusters(cIndex, neighIdx[i], 
			/*child1, child2, */&(mergeNode->mergeCluster));
//		if(bnnode)
//		AfxMessageBox("C");

		mergeNode->childIndex1 = cIndex;
		mergeNode->childIndex2 = neighIdx[i];
		// totalSecondMoment = mergeNode->mergeCluster.GetUvwSecondMoment();

		/*
		uError = mergeNode->mergeCluster.GetuError();
		vError = mergeNode->mergeCluster.GetvError();
		wError = mergeNode->mergeCluster.GetwError();
		*/

		uvwError = mergeNode->mergeCluster.GetUvwError();

		// mod 7Aug03
		neighList->AddNode((uvwError)-oldError , (void*) mergeNode);
		//neighList->AddNode(totalSecondMoment-oldSecondMoment , (void*) mergeNode);


	}
		
	return;

}




////////////////////////////////////////////////////////////////////
//									
////////////////////////////////////////////////////////////////////

int FlowAbstraction::UpdateErrorList(CSortedList *errorList, CSortedList *neighList, int cIndex)
{
	//
	// return the no. of nodes copied
	//

	int		nCopied;

	nCopied = errorList->MoveNodesFromList(neighList);

#if DEBUG
	if (!neighList->IsEmpty())
		AfxMessageBox("ERROR in UpdateErrorList ");
#endif

	return nCopied;

}



////////////////////////////////////////////////////////////////////
//									RetrieveDenseVectors()
////////////////////////////////////////////////////////////////////

int FlowAbstraction::RetrieveDenseVectors(float compressRatio, CVector3D *pVecs)
{
	if (m_rootCluster == NULL)
		return 0;

	int						nCluster;
	int						maxLevel;
	int						topLevel;	
	
	nCluster = (int) ((1 - compressRatio/100.0) * m_vecSize + 0.5);
	topLevel = m_rootCluster->GetLevel();
	
	if (nCluster >= m_nLeafCluster){
		maxLevel = 0;
		nCluster = m_nLeafCluster;
	}
	else {
		maxLevel =(topLevel + 1 ) - nCluster;
	}

	int nVec = SearchClusters(m_rootCluster, maxLevel, pVecs);
	ASSERT (nVec == m_vecSize);
	
	return nVec;
}


////////////////////////////////////////////////////////////////////
//									SearchClusters()
////////////////////////////////////////////////////////////////////
//
// Recursive function called by RetrieveDenseVectors()
//

int FlowAbstraction::SearchClusters(HCluster *root, int maxLevel, CVector3D *pVecs)
{
	int	nVec;
	int nVec1, nVec2;

	if ( root->GetLevel() > maxLevel ) {
		nVec1 = SearchClusters(root->GetFirstChild(), maxLevel, pVecs);
		nVec2 = SearchClusters(root->GetSecondChild(), maxLevel, pVecs);
		nVec = nVec1 + nVec2;
	}
	else{

		int		idx;
		int		n;
		FlowCluster *fCluster;
		
		fCluster = (FlowCluster*) root->GetCluster();
		nVec = fCluster->GetNPts();
		CPoint3D *pts = new CPoint3D[nVec];
		fCluster->GetPts(pts);
		
		if (nVec > 1) {
			for ( n = 0; n < nVec; n++){
				idx = pts[n].z * m_vecSizeX * m_vecSizeY + pts[n].y * m_vecSizeX + pts[n].x;
				pVecs[idx] = fCluster->GetVectorAt(pts[n].x, pts[n].y, pts[n].z);
			}
		}
		else {
			idx = pts[0].z * m_vecSizeX * m_vecSizeY + pts[0].y * m_vecSizeX + pts[0].x;
			pVecs[idx] = m_oriVec[idx];
		}

		delete [] pts;
	}

	return nVec;
}



////////////////////////////////////////////////////////////////////
//									RetrieveClusters()
////////////////////////////////////////////////////////////////////

int FlowAbstraction::RetrieveClusters(float compressRatio, FlowCluster* fClusters,  
		int fCluster_size, int *clusterMap, CVector3D *pVecs)
{
	if (m_rootCluster == NULL)
		return 0;

	int						nCluster;
	int						currIdx;
	int						maxLevel;
	int						topLevel;	
	
	nCluster = (int) ((1 - compressRatio/100.0) * m_vecSize + 0.5);
	topLevel = m_rootCluster->GetLevel();
	
	if (nCluster >= m_nLeafCluster)
	{
		maxLevel = 0;
		nCluster = m_nLeafCluster;
	}
	else 
		maxLevel =(topLevel + 1 ) - nCluster;

	if(nCluster != fCluster_size)
	{
		CString sms233;
		sms233.Format("Error in RetrieveClusters(): nCluster = %i, fCluster_size = %i.", 
			nCluster, fCluster_size);
		AfxMessageBox(sms233);
	}
	
	currIdx = 0;

	// MemSave comment
	/*
	int nVec = SearchAndRetrieveClusters(m_rootCluster, fClusters, currIdx, maxLevel, clusterMap);
	ASSERT (currIdx == nCluster && nVec == m_vecSize);
	
	
	int idx;
	int	x, y, z;

	for ( z = 0; z < m_vecSizeZ; z++)
		for ( y = 0; y < m_vecSizeY; y++)
			for ( x = 0; x < m_vecSizeX; x++)
			{
				idx = z * m_vecSizeX * m_vecSizeY + y * m_vecSizeX + x;
				if (fClusters[clusterMap[idx]].GetNPts() == 1) 
					pVecs[idx] = m_oriVec[idx];
				else
					pVecs[idx] = fClusters[clusterMap[idx]].GetVectorAt(x,y,z);
			}

    */

	int x, y, z;
	for ( z = 0; z < m_vecSizeZ; z++)
		for ( y = 0; y < m_vecSizeY; y++)
			for ( x = 0; x < m_vecSizeX; x++)
				clusterMap[z * m_vecSizeX * m_vecSizeY + y * m_vecSizeX + x] = NO_CLUSTER;

	// fill in fClusters with cluster data (not pointers) of clusters below maxLevel
	// fill clusterMap with index of child-most cluster at hit position
	// fill pVecs with fit vector of parent-most cluster at hit position
	int node_idx;
	int fCluster_entry = 0, fCluster_found = -1;
	HCluster* maxParent;
	CPoint3D leafCentroid;
	for(node_idx = 0; node_idx < m_nLeafCluster; node_idx++)
	{
		// search for leaf node's highest parent at or below maxLevel
		if(nCluster>1)
			maxParent = SearchLeafNodeHighestParent(m_allCluster[node_idx], maxLevel);
		else
			maxParent = m_allCluster[m_nLeafCluster-1];

		if(maxParent == NULL)
			maxParent = m_allCluster[node_idx];

		// get maxParent index if it's already in fClusters
		fCluster_found = SearchClusterInArray((FlowCluster*)maxParent->GetCluster(), 
			fClusters, fCluster_entry);

		if(fCluster_found == -1)
		{

			if(fCluster_entry >= nCluster)
			{
				// AfxMessageBox("Error A in RetrieveClusters(): fCluster_entry >= nCluster.");
				continue;
			}

			fCluster_found = fCluster_entry;

			// increment current fCluster entry index for next entry
			fCluster_entry++;

			// set current fClusters entry to highest parent's data
			FlowClusterCopy((FlowCluster*)maxParent->GetCluster(), &fClusters[fCluster_found]);
			fClusters[fCluster_found].pClusterOriginal = (FlowCluster*)maxParent->GetCluster();
		}

		if(fCluster_found >= nCluster)
		{
			AfxMessageBox("Error B in RetrieveClusters(): fCluster_found >= nCluster.");
			break;
		}
	
		// set clusterMap with fCluster index of parent at leaf node's centroid position
		leafCentroid = ((FlowCluster*)m_allCluster[node_idx]->GetCluster())->GeoCentroid();

		if(	leafCentroid.x < 0 || leafCentroid.x >= m_vecSizeX ||
			leafCentroid.y < 0 || leafCentroid.y >= m_vecSizeY ||
			leafCentroid.z < 0 || leafCentroid.z >= m_vecSizeZ )
		{
			CString sms133;
			sms133.Format("Error: centroid pos (%i, %i, %i) with limits (%i, %i, %i).",
				leafCentroid.x, leafCentroid.y, leafCentroid.z, 
				m_vecSizeX, m_vecSizeY, m_vecSizeZ);
			AfxMessageBox(sms133);

			return 0;
		}

		clusterMap[leafCentroid.z*m_vecSizeX*m_vecSizeY + leafCentroid.y*m_vecSizeX + leafCentroid.x]
			= fCluster_found;

		// set pVecs to fit vector from maxLevel parent from leaf node
		pVecs[leafCentroid.z*m_vecSizeX*m_vecSizeY + leafCentroid.y*m_vecSizeX + leafCentroid.x] = 
			fClusters[fCluster_found].GetVectorAt(leafCentroid.x,leafCentroid.y,leafCentroid.z);


	}

/*

	int idx;
	int	x, y, z;

	for ( z = 0; z < m_vecSizeZ; z++)
		for ( y = 0; y < m_vecSizeY; y++)
			for ( x = 0; x < m_vecSizeX; x++)
			{
				idx = z * m_vecSizeX * m_vecSizeY + y * m_vecSizeX + x;
				posCluster = GetLevelClusterAt(x, y, z, maxLevel);
				clusterMap[idx] = posCluster;
				pVecs[idx] = fClusters[posCluster].GetVectorAt(x,y,z);
			}

	return nCluster;
*/

	return fCluster_entry;
}



////////////////////////////////////////////////////////////////////
//									SearchAndRetrieveClusters()
////////////////////////////////////////////////////////////////////
//
// Recursive function called by RetrieveClusters()
//

int FlowAbstraction::SearchAndRetrieveClusters(HCluster *root, FlowCluster *fClusters, 
	int &currIdx, int maxLevel, int *clusterMap)
{
	int	nVec;
	int nVec1, nVec2;
	static CPoint3D *pts = new CPoint3D[m_vecSizeX*m_vecSizeY*m_vecSizeZ];

	if ( root->GetLevel() > maxLevel ) 
	{
		nVec1 = SearchAndRetrieveClusters(root->GetFirstChild(), fClusters, currIdx, maxLevel, clusterMap);
		nVec2 = SearchAndRetrieveClusters(root->GetSecondChild(), fClusters, currIdx, maxLevel, clusterMap);
		nVec = nVec1 + nVec2;
	}
	else
	{

		FlowClusterCopy((FlowCluster*)root->GetCluster(), fClusters + currIdx);
		
		int		n;
		nVec = fClusters[currIdx].GetNPts();
		CPoint3D *pts = new CPoint3D[m_vecSize];
		fClusters[currIdx].GetPts(pts);
		for ( n = 0; n < nVec; n++)
		{
			clusterMap[pts[n].z * m_vecSizeX * m_vecSizeY + pts[n].y * m_vecSizeX + pts[n].x] = currIdx;
		}
		delete [] pts;
		currIdx++;

	}

	return nVec;
}



HCluster* FlowAbstraction::SearchLeafNodeHighestParent(HCluster *leafCluster, int maxLevel)
{
	HCluster *overParent = NULL, *underParent = NULL;
	overParent = leafCluster->GetParent();
	while(overParent->GetLevel() <= maxLevel)
	{
		underParent = overParent;
		overParent = underParent->GetParent();
		if(overParent == NULL)
		{
			AfxMessageBox("Error: NULL parent in SearchLeafNodeHighestParent()");
			break;
		}

	}

	return underParent;
}

int FlowAbstraction::SearchClusterInArray(FlowCluster *clusterFind, FlowCluster *hcArray, int max_idx)
{
	static int i;

	for(i = 0; i<max_idx; i++)
		if(clusterFind == hcArray[i].pClusterOriginal)
		{
			return i;
		}

	return -1;
}

void FlowAbstraction::SaveClusterData(CString filename)
{
	// create file
	ofstream of;
	of.open(filename, ios::out);

	// save array and image sizes to file
	of << m_nCluster << " " << m_vecSizeX << " " << m_vecSizeY << " " << m_vecSizeZ << "\n";

	// save entire m_allCluster contents to file
	FlowCluster* pfcluster;
	CPlane* pFitPlane;
	CVector3D vAverageVel;
	float** pMatrixA;
	float* v0;

	int node_idx;
	for(node_idx = 0; node_idx < m_nCluster; node_idx++)
	{
		// HCluster child1, child2, parent, level
		of	<< GetClusterPointerIndex(m_allCluster[node_idx]->GetParent()) << " "
			<< GetClusterPointerIndex(m_allCluster[node_idx]->GetFirstChild()) << " "
			<< GetClusterPointerIndex(m_allCluster[node_idx]->GetSecondChild()) << " "
			<< m_allCluster[node_idx]->GetLevel() << "\n";

		// FlowCluster m_b3D, centroid coords, m_uvwError
		pfcluster = (FlowCluster*)m_allCluster[node_idx]->GetCluster();
		of	<< pfcluster->m_b3D << " "
			<< pfcluster->GeoCentroid().x << " "
			<< pfcluster->GeoCentroid().y << " "
			<< pfcluster->GeoCentroid().z << " "
			<< pfcluster->GetUvwError() << "\n";

		pFitPlane = pfcluster->GetFitPlanes();
		vAverageVel = pFitPlane->GetAverageV();

		// CPlane m_lmsAverageV
		of	<< vAverageVel.x << " "
			<< vAverageVel.y << " "
			<< vAverageVel.z << "\n";

		pMatrixA = pFitPlane->GetMatrixA();
		if(pMatrixA==NULL)
		{
			AfxMessageBox("Error: NULL matrix_a in saving CPlane data.");
		}

		// CPlane matrix_a
		if(pfcluster->m_b3D)
		{
			of	<< pMatrixA[1][1] << " "
				<< pMatrixA[1][2] << " "
				<< pMatrixA[1][3] << " "
				<< pMatrixA[2][1] << " "
				<< pMatrixA[2][2] << " "
				<< pMatrixA[2][3] << " "
				<< pMatrixA[3][1] << " "
				<< pMatrixA[3][2] << " "
				<< pMatrixA[3][3] << "\n";
		}
		else
		{
			of	<< pMatrixA[1][1] << " "
				<< pMatrixA[1][2] << " "
				<< pMatrixA[2][1] << " "
				<< pMatrixA[2][2] << "\n";
		}

		// CPlane v0
		v0 = pFitPlane->GetV0();
		of	<< v0[0] << " " << v0[1] << " " << v0[2] << "\n";

	}


}

BOOL FlowAbstraction::LoadClusterData(CString filename)
{
	// load entire m_allCluster contents from file
	ifstream inf;
	inf.open(filename, ios::in);

	// delete m_allCluster
	if ( m_allCluster != NULL)
	{
		for(int i = 0; i < m_nCluster; i++)
			delete m_allCluster[i];

		delete [] m_allCluster;
	}

	// get array size from file
	inf >> m_nCluster >> m_vecSizeX >> m_vecSizeY >> m_vecSizeZ;

	m_nLeafCluster = m_vecSizeX * m_vecSizeY * m_vecSizeZ;
	m_nNonLeafCluster = m_nLeafCluster - 1;
	m_nCluster = m_nLeafCluster + m_nNonLeafCluster;
	m_vecSize	 = m_nLeafCluster;

	// allocate new m_allCluster
	m_allCluster = new HCluster*[m_nCluster];
	for(int i = 0; i < m_nCluster; i++)
		m_allCluster[i] = new HCluster();

	m_rootCluster =  m_allCluster[m_nCluster - 1];

	// load entire m_allCluster contents from file
	int node_idx;
	int parent, child1, child2, level;
	FlowCluster* pfcluster;
	int cl_b3D, cl_cx, cl_cy, cl_cz; 
	double cl_uvwError;
	float cp_av_vx, cp_av_vy, cp_av_vz;
	CPlane* cl_fitPlane;
	float** cp_MatrixA = matrix(1, 3, 1, 3);
	float v0[3];
	for(node_idx = 0; node_idx < m_nCluster; node_idx++)
	{
		// HCluster child1, child2, parent, level
		inf	>> parent >> child1 >> child2 >> level;

		m_allCluster[node_idx]->SetValuesFromFileData(m_allCluster[child1],
			m_allCluster[child2], m_allCluster[parent], level);

		pfcluster = (FlowCluster*)m_allCluster[node_idx]->GetCluster();

		// FlowCluster m_b3D, centroid coords, m_uvwError
		inf >> cl_b3D >> cl_cx >> cl_cy >> cl_cz >> cl_uvwError;
		pfcluster->SetValuesFromFileData(cl_b3D, cl_cx, cl_cy, cl_cz, cl_uvwError);

		// CPlane m_lmsAverageV, matrix_a, v0
		cl_fitPlane = pfcluster->GetFitPlanes();
		inf >> cp_av_vx >> cp_av_vy >> cp_av_vz;

		if(cl_b3D==TRUE)
		{
			inf >> cp_MatrixA[1][1]
				>> cp_MatrixA[1][2]
				>> cp_MatrixA[1][3]
				>> cp_MatrixA[2][1]
				>> cp_MatrixA[2][2]
				>> cp_MatrixA[2][3]
				>> cp_MatrixA[3][1]
				>> cp_MatrixA[3][2]
				>> cp_MatrixA[3][3];
		}
		else
		{
			inf >> cp_MatrixA[1][1]
				>> cp_MatrixA[1][2]
				>> cp_MatrixA[2][1]
				>> cp_MatrixA[2][2];
		}

		inf >> v0[0] >> v0[1] >> v0[2];

		cl_fitPlane->SetValuesFromFileData(cl_b3D, cp_av_vx, cp_av_vy, cp_av_vz, cp_MatrixA, v0, 
			cl_cx, cl_cy, cl_cz);
		
	}
	
	free_matrix(cp_MatrixA, 1, 3, 1, 3);

	return 0;
}

int FlowAbstraction::GetClusterPointerIndex(HCluster *pHCluster)
{
	int i, result = 0;
	for(i=0; i<m_nCluster; i++)
		if(m_allCluster[i] == pHCluster)
			result = i;

	return result;
}

void FlowAbstraction::GetVecSizes(int &vx, int &vy, int &vz)
{
	vx = m_vecSizeX;
	vy = m_vecSizeY;
	vz = m_vecSizeZ;
}

void FlowAbstraction::MemSaveGetSamplePoints(HCluster* root, CPoint3D *samplePts, int &nSamplePts)
{
	CPoint3D* sample_pts;
	int i, n_sample_pts;

	if(root->IsLeaf()==TRUE)
	{
		MemSaveGetSamplePoints(root->GetFirstChild(), samplePts, nSamplePts);
		MemSaveGetSamplePoints(root->GetSecondChild(), samplePts, nSamplePts);
	}
	else
	{
		if(samplePts!=NULL)
		{
			n_sample_pts = 
				MemSaveGetSamplePoints(((FlowCluster*)root->GetCluster())->GeoCentroid(), NULL);
			sample_pts = new CPoint3D[n_sample_pts];
			MemSaveGetSamplePoints(((FlowCluster*)root->GetCluster())->GeoCentroid(), sample_pts);
			for(i=0; i<n_sample_pts; i++)
				samplePts[nSamplePts+i] = sample_pts[i];
			nSamplePts = nSamplePts + n_sample_pts;
			delete [] sample_pts;
		}
		else
			nSamplePts = nSamplePts 
			+ MemSaveGetSamplePoints(((FlowCluster*)root->GetCluster())->GeoCentroid(), NULL);
	}

}

void FlowAbstraction::MemSaveGetClusterPoints(HCluster* root, CPoint3D *nodePts, int &nNodePts)
{
	if(root->IsLeaf()==TRUE)
	{
		MemSaveGetClusterPoints(root->GetFirstChild(), nodePts, nNodePts);
		MemSaveGetClusterPoints(root->GetSecondChild(), nodePts, nNodePts);
	}
	else
	{
		if(nodePts!=NULL)
		{
			nodePts[nNodePts] = ((FlowCluster*)root->GetCluster())->GeoCentroid();
			nNodePts++;
		}
		else
			nNodePts++;
	}

}

int FlowAbstraction::MemSaveGetSamplePoints(CPoint3D centre, CPoint3D *sample_pts)
{
	int x, y, z;
	int iSamplePt = 0;

	for(x=centre.x - 1; x <= centre.x + 1; x++)
		for(y=centre.y - 1; y <= centre.y + 1; y++)
			for(z=centre.z - 1; z <= centre.z + 1; z++)
			{
				if(x >= 0 && y >= 0 && z >= 0 && 
				   x < m_vecSizeX && y < m_vecSizeY && z < m_vecSizeZ && 
				   !(x== centre.x && y == centre.y && z == centre.z) &&
				   m_oriVec[z*m_vecSizeX*m_vecSizeY+y*m_vecSizeX+x].Norm()>0.0)
				{
					if(sample_pts!=NULL)
					{
						sample_pts[iSamplePt].x = x;
						sample_pts[iSamplePt].y = y;
						sample_pts[iSamplePt].z = z;
					}
					iSamplePt++;
				}
				else
					continue;
			}

	return 	iSamplePt;

}

int FlowAbstraction::GetClusterCount()
{
	return m_nCluster;
}

void FlowAbstraction::RetrieveOriginalVelocities(CVector3D *pVecs)
{
	for(int node_idx = 0; node_idx < m_nLeafCluster; node_idx++)
	{
		CPoint3D leafCentroid = ((FlowCluster*)m_allCluster[node_idx]->GetCluster())->GeoCentroid();

		if( leafCentroid.x >= 0 && leafCentroid.x < m_vecSizeX &&
			leafCentroid.y >= 0 && leafCentroid.y < m_vecSizeY &&
			leafCentroid.z >= 0 && leafCentroid.z < m_vecSizeZ )
			pVecs[leafCentroid.z*m_vecSizeX*m_vecSizeY + leafCentroid.y*m_vecSizeX + leafCentroid.x] = 
				((FlowCluster*)m_allCluster[node_idx]->GetCluster())->GetFitPlanes()->GetAverageV();

	}
	
}
