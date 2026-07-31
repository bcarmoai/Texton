// FlowCluster.cpp: implementation of the FlowCluster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "FlowCluster.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FlowCluster::FlowCluster()
{
	m_nPts = 0;
	m_nSamplePts = 0;
	m_samplePts = NULL;
	m_pts = NULL;
	m_vecs = NULL;
	m_sampleVecs = NULL;
	m_b3D = TRUE;

}

FlowCluster::FlowCluster(FlowCluster &fc)
{
	// copy constructor

	/*
	m_uPlane			= fc.m_uPlane;
	m_vPlane			= fc.m_vPlane;
	m_wPlane			= fc.m_wPlane;
	*/

	m_FitPlanes = fc.m_FitPlanes;
	m_b3D = fc.m_b3D;

	/*
	m_uError			= fc.m_uError;
	m_vError			= fc.m_vError;
	m_wError			= fc.m_wError;
	*/
	m_uvwError			= fc.m_uvwError;

	/*
	m_uSecondMoment = fc.m_uSecondMoment;
	m_vSecondMoment = fc.m_vSecondMoment;
	m_wSecondMoment = fc.m_wSecondMoment;
	*/
	
	m_nPts			= fc.m_nPts;
	m_nSamplePts	= (fc.m_samplePts != NULL ? fc.m_nSamplePts : 0);
	m_centroid		= fc.m_centroid;
	m_boundRect		= fc.m_boundRect;

	int i;
	m_pts = new CPoint3D[m_nPts];
	for (i = 0; i < m_nPts; i++)
		m_pts[i] = fc.m_pts[i];

	m_vecs = new CVector3D[m_nPts];
	for (i = 0; i < m_nPts; i++)
		m_vecs[i] = fc.m_vecs[i];
	
	if ( m_nSamplePts > 0 && fc.m_samplePts != NULL) {
		m_samplePts = new CPoint3D[m_nSamplePts];
		for (i = 0; i < m_nSamplePts; i++)
			m_samplePts[i] = fc.m_samplePts[i];
	}
	else
		m_samplePts = NULL;

	if ( m_nSamplePts > 0) {
		m_sampleVecs = new CVector3D[m_nSamplePts];
		for (i = 0; i < m_nSamplePts; i++)
			m_sampleVecs[i] = fc.m_sampleVecs[i];
	}
	else
		m_sampleVecs = NULL;

	if(m_centroid.x < 0)
		AfxMessageBox("Centroid error");
}



FlowCluster::~FlowCluster()
{
	if(m_nPts>0)
	{
		if ( m_pts != NULL)
			delete [] m_pts;
		if( m_vecs != NULL)
			delete [] m_vecs;
	}

	if(m_nSamplePts>0)
	{
		if ( m_samplePts != NULL)
			delete [] m_samplePts;
		if ( m_sampleVecs != NULL)
			delete [] m_sampleVecs;
	}



}


//////////////////////////////////////////////////////////////////////
// GeoCentroid()
//////////////////////////////////////////////////////////////////////
CPoint3D FlowCluster::GeoCentroid()
{
	// find the geometric centroid of the cluster
	return m_centroid;
}

CVector3D FlowCluster::GetVectorAt(int x, int y, int z)
{
	return m_FitPlanes.GetFitVel(x, y, z);
}


CVector3D FlowCluster::RepVector()
{
	return GetVectorAt(m_centroid.x, m_centroid.y, m_centroid.z);
}




//////////////////////////////////////////////////////////////////////
//	Initialise()
//////////////////////////////////////////////////////////////////////

int FlowCluster::InitialiseCluster(CPoint3D *coor, CVector3D *vec, int nPts, 
							CPoint3D *sampleCoor, CVector3D *sampleVec, 
							int nSamplePts, BOOL b3D)
{
	CPoint3D	*pts3D;
	CVector3D   *vcs3D;
	int			i, idx;
	int			total;

	m_b3D = b3D;
	total = nPts + nSamplePts;
	pts3D = new CPoint3D[total];	// concatenated coordinates
	vcs3D = new CVector3D[total];	// concatenated vectors

	m_nPts = nPts;
	m_nSamplePts = nSamplePts;					// cluster points
	m_pts = new CPoint3D[nPts];					// cluster vectors;
	m_vecs = new CVector3D[nPts];				// sample points
	m_samplePts = new CPoint3D[nSamplePts];		// sample vectors
	m_sampleVecs = new CVector3D[nSamplePts];
	
	// concatenate original and sample points
	// and copy argument points and vectors into own vars
	for ( i = 0; i < nPts; i++)
	{
		pts3D[i] = coor[i];
		m_pts[i] = pts3D[i];
		m_vecs[i] = vcs3D[i] = vec[i];
	}

	for ( i = 0, idx=nPts; i < nSamplePts; i++, idx++)
	{
		pts3D[idx] = sampleCoor[i];
		m_samplePts[i] = pts3D[idx];
		m_sampleVecs[i] = vcs3D[idx] = sampleVec[i];
	}

	// fit 3 planes to 3D data
	if(m_FitPlanes.LeastSquareFitting(m_b3D, pts3D, vcs3D, total)==0)
	{
		AfxMessageBox("LeastSquareFitting call failed in FlowCluster::InitialiseCluster().");
		return 0;
	}

	if(nPts!=1)
		m_centroid = m_FitPlanes.GetCentroid();
	else
		m_centroid = coor[0];

	// get velocity error about each position
	m_uvwError = 0;
	for ( i = 0; i < nPts; i++)
		m_uvwError += m_FitPlanes.SqDistFromPt(pts3D[i], vcs3D[i]);
	
	// compute boundingRect from the clusterPts
	InitBoundingRect();

	delete [] pts3D;
	delete [] vcs3D;

	return 1;
}



///////////////////////////////////////////////////////////////////
//		GetPts()
///////////////////////////////////////////////////////////////////

int	FlowCluster::GetPts(CPoint3D *pts)
{
	int i;
	
	if ( m_nPts >0){

		for (i = 0; i < m_nPts; i++)
			pts[i] = m_pts[i];
	}
	else
		i = 0;

	return i;
}


///////////////////////////////////////////////////////////////////
//		GetSamplePts()
///////////////////////////////////////////////////////////////////

int FlowCluster::GetSamplePts(CPoint3D *samplePts)
{
	if(m_samplePts!=NULL)
	{
		int		i;

		if ( m_nSamplePts > 0) {
			for (i = 0; i < m_nSamplePts; i++)
				samplePts[i] = m_samplePts[i];
		}
		else
			i = 0;
		return i;
	}
	else
		return 0;
}	


///////////////////////////////////////////////////////////////////
//		PtInCluster()
///////////////////////////////////////////////////////////////////

bool	FlowCluster::PtInCluster(CPoint3D aPt)
{
	int i;
	bool	found = false;
	
	if ( m_nPts >0 && m_boundRect.PtInRect(aPt)){


		for (i = 0; i < m_nPts; i++)
			if ( m_pts[i] == aPt ) {
				found = true;
				break;
			}
	}

	return found;
	
}


///////////////////////////////////////////////////////////////////
//  definition of friend function
///////////////////////////////////////////////////////////////////

bool FlowClusterCopy(const FlowCluster* fc, FlowCluster* aCluster)
{
	aCluster->m_FitPlanes			= fc->m_FitPlanes;

	/*
	aCluster->m_vPlane				= fc->m_vPlane;
	aCluster->m_wPlane				= fc->m_wPlane;
	*/

	/*
	aCluster->m_vError				= fc->m_vError;
	aCluster->m_wError				= fc->m_wError;
	*/
	
	// aCluster->m_uvwSecondMoment = fc->m_uvwSecondMoment;
	/*
	aCluster->m_vSecondMoment = fc->m_vSecondMoment;
	aCluster->m_wSecondMoment = fc->m_wSecondMoment;
	*/
	aCluster->m_uvwError			= fc->m_uvwError;

	aCluster->m_nPts				= fc->m_nPts;
	aCluster->m_nSamplePts			= (fc->m_samplePts != NULL ? fc->m_nSamplePts : 0);
	
	aCluster->m_centroid			= fc->m_centroid;
	aCluster->m_boundRect			= fc->m_boundRect;
	
	int i;
	aCluster->m_pts = new CPoint3D[aCluster->m_nPts];
	for (i = 0; i < aCluster->m_nPts; i++)
		aCluster->m_pts[i] = fc->m_pts[i];
	
	if ( fc->m_nSamplePts > 0) {
		aCluster->m_samplePts = new CPoint3D[aCluster->m_nSamplePts];
		for (i = 0; i < aCluster->m_nSamplePts; i++)
			aCluster->m_samplePts[i] = fc->m_samplePts[i];
	}

	aCluster->m_vecs = new CVector3D[aCluster->m_nPts];
	for (i = 0; i < aCluster->m_nPts; i++)
		aCluster->m_vecs[i] = fc->m_vecs[i];
	
	if ( aCluster->m_nSamplePts > 0 && fc->m_samplePts != NULL) 
	{
		aCluster->m_sampleVecs = new CVector3D[aCluster->m_nSamplePts];
		for (i = 0; i < aCluster->m_nSamplePts; i++)
			aCluster->m_sampleVecs[i] = fc->m_sampleVecs[i];
	}
	else
		aCluster->m_sampleVecs = NULL;


	if(aCluster->m_centroid.x < 0)
		AfxMessageBox("Centroid error");

	return true;
}


void FlowCluster::InitBoundingRect()
{
	int			i;
	int			maxX, maxY, maxZ;
	int			minX, minY, minZ;
	double		sumX, sumY, sumZ;
	int			total;

	total = m_nPts + m_nSamplePts;
	
	maxX = minX = m_pts[0].x;
	maxY = minY = m_pts[0].y;
	maxZ = minZ = m_pts[0].z;
	sumX = m_pts[0].x;
	sumY = m_pts[0].y;
	sumZ = m_pts[0].z;

	for ( i = 1; i < m_nPts; i++)
	{
			sumX += m_pts[i].x;
			sumY += m_pts[i].y;
			sumZ += m_pts[i].z;

			if ( m_pts[i].x > maxX)
				maxX = m_pts[i].x;
			else if ( m_pts[i].x < minX)
				minX = m_pts[i].x;

			if ( m_pts[i].y > maxY)
				maxY = m_pts[i].y;
			else if ( m_pts[i].y < minY)
				minY = m_pts[i].y;

			if ( m_pts[i].z > maxZ)
				maxZ = m_pts[i].z;
			else if ( m_pts[i].z < minZ)
				minZ = m_pts[i].z;
	}

	if(minZ<0 || maxZ<0 || minX<0 || maxX<0 || minY<0 || maxY<0)
		AfxMessageBox("InitBoundingRect error");

	m_boundRect = CRect3D(minX, minY, maxX+1, maxY + 1, minZ, maxZ + 1);

}

void FlowCluster::MemSaveDeletePoints()
{
	// MemSave START 27/1/04
	if ( m_pts != NULL)
		delete [] m_pts;
	m_pts = NULL;
	if(m_nSamplePts>0)
	{
		if ( m_samplePts != NULL)
			delete [] m_samplePts;
		if(m_sampleVecs!=NULL)
			delete [] m_sampleVecs;
	}
	m_samplePts = NULL;
	m_nSamplePts = 0;  
	m_nPts = 0;
	// MemSave END 27/1/04
	if(m_vecs!=NULL)
		delete [] m_vecs;

}

CPlane* FlowCluster::GetFitPlanes()
{ 
	return &m_FitPlanes; 
}

void FlowCluster::SetValuesFromFileData(BOOL b3D, int cx, int cy, int cz, double uvwError)
{
	m_b3D = b3D;
	m_centroid.x = cx;
	m_centroid.y = cy;
	m_centroid.z = cz;
	m_uvwError = uvwError;

}

