// FlowAbstraction.h: interface for the FlowAbstraction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOWABSTRACTION_H__45819D18_9352_41E4_A2A2_94D03F02C338__INCLUDED_)
#define AFX_FLOWABSTRACTION_H__45819D18_9352_41E4_A2A2_94D03F02C338__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Vector3D.h"
#include "HCluster.h"
#include "SortedList.h"
#include "FlowCluster.h"
#include "Point3D.h"

#define NO_CLUSTER -1

class FlowAbstraction  
{
public:
	FlowAbstraction();
	virtual ~FlowAbstraction();

public:
	int GetClusterCount();
	void GetVecSizes(int& vx, int& vy, int& vz);
	BOOL LoadClusterData(CString filename);
	void SaveClusterData(CString filename);
	void RetrieveOriginalVelocities(CVector3D *pVecs);
	int SearchClusterInArray(FlowCluster* clusterFind, FlowCluster* hcArray, int max_idx);

	int					RetrieveClusters(float compressRatio, FlowCluster* fCluster, int fCluster_size, 
										 int *clusterMap, CVector3D *pVecs);
	int					RetrieveDenseVectors(float compressionRatio, CVector3D *pVecs);

	bool				StartClustering();
	BOOL				Initialise(CVector3D *vec, int sizeX, int sizeY, int sizeZ);

private:

	int					SearchClusters(HCluster *root, int maxLevel, CVector3D *pVecs);
	int					SearchAndRetrieveClusters(HCluster *root, FlowCluster *fClusters, int &currIdx, int maxLevel, int *clusterMap);

	int					UpdateErrorList(CSortedList* errorList, CSortedList* neighList, int cIndex);
	void				CalNeighError(int cIndex,int *neighIdx, int nNeigh, CSortedList * neighList);
	
	int					SearchListForIndex(CSortedList *errorList, int searchIdx, int *neighIdx, int nNeigh);
	int					EliminateDuplicateIndex(int *index, int n1, int n2, int *neighCount);

	void				MergeClusters(int fc1, int fc2, FlowCluster* fc);
	
	CSortedList* CreateErrorList(int* neighCount, int** neighIdxs);
	
	int					CreateLeafClusters();
	
	void				ClearClusters();

	CVector3D		*m_oriVec;
	int					m_vecSizeY;
	int					m_vecSizeX;
	int					m_vecSizeZ;
	int					m_vecSize;

	HCluster		**m_allCluster;
	HCluster		**m_leafCluster;
	HCluster		**m_nonLeafCluster;
	
	HCluster*		m_rootCluster;

	int					m_nCluster;
	int					m_nLeafCluster;
	int					m_nNonLeafCluster;

	bool				m_initialised;

protected:
	int m_zeroSkipSize;
	int  MemSaveGetSamplePoints(CPoint3D centre, CPoint3D* sample_pts);
	void MemSaveGetClusterPoints(HCluster* root, CPoint3D* nodePts, int &nNodePts);
	void MemSaveGetSamplePoints(HCluster* root, CPoint3D* samplePts, int &nSamplePts);
	int* m_ptsMap3D;
	int GetClusterPointerIndex(HCluster* pHCluster);
	FlowCluster* testCluster;
	HCluster* SearchLeafNodeHighestParent(HCluster* leafCluster, int maxLevel);
	BOOL bnnode;
};

#endif // !defined(AFX_FLOWABSTRACTION_H__45819D18_9352_41E4_A2A2_94D03F02C338__INCLUDED_)
