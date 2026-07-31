// FlowCluster.h: interface for the FlowCluster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLOWCLUSTER_H__B422AF88_A2DD_4C98_9EBC_479C23EF3382__INCLUDED_)
#define AFX_FLOWCLUSTER_H__B422AF88_A2DD_4C98_9EBC_479C23EF3382__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Plane.h"
#include "Vector3D.h"	// Added by ClassView
#include "Point3D.h"
#include "Rect3D.h"

class FlowCluster  
{
public:
	void SetValuesFromFileData(BOOL b3D, int cx, int cy, int cz, double uvwError);
	FlowCluster* pClusterOriginal;
	void MemSaveDeletePoints();
	BOOL m_b3D;

	bool			PtInCluster(CPoint3D aPt);

	// functions for retrieving data 

	CPlane*			GetFitPlanes(); 
	/*
	CPlane			GetUPlane() { return m_vPlane; };
	CPlane			GetVPlane() { return m_vPlane; };
	CPlane			GetWPlane() { return m_wPlane; };
	*/

	/*
	double			GetuError() { return m_uError; };
	double			GetvError() { return m_vError; };
	double			GetwError() { return m_wError; };
	*/
	double			GetUvwError() { return m_uvwError; };

	// double		GetUvwSecondMoment() {return m_uvwSecondMoment;};
	
	CRect3D			BoundRect() {	return m_boundRect;} ;

	int				GetNPts() { return m_nPts;} ;
	int				GetNSamplePts() 
	{ 
		return m_nSamplePts; 
	} ;

	int				GetPts(CPoint3D *pts);
	int				GetSamplePts(CPoint3D *samplePts);


	int				InitialiseCluster(CPoint3D *coor, CVector3D *vec, int nPts, 
							CPoint3D *sampleCoor = NULL, CVector3D *sampleVec = NULL, 
							int nSamplePts = 0, BOOL b3D = TRUE);
	CVector3D		RepVector();
	CVector3D		GetVectorAt(int x, int y, int z);
	CPoint3D		GeoCentroid();

	// constructors & destructors
	FlowCluster();
	FlowCluster(FlowCluster &fc);
	//FlowCluster(CPlane uPlane, CPlane vPlane, double uSecondMoment, double vSecondMoment, 
	//	int nPts, CPoint centroid, CRect boundRect);
	virtual ~FlowCluster();

protected:
	void InitBoundingRect();
	
	int			m_nPts;
	int			m_nSamplePts;

	CPlane	m_FitPlanes;
	// CPlane	m_uPlane;
	// CPlane	m_vPlane;
	// CPlane	m_wPlane;
	// 28/11/03 by BSC; one plane now contains the gradients of all 3 fitted planes

	/*
	double		m_uSecondMoment;
	double		m_vSecondMoment;
	double		m_wSecondMoment;
	*/
	// double		m_uvwSecondMoment;

	double		m_uvwError;
	/*
	double		m_uError;
	double		m_vError;
	double		m_wError;
	*/

	CRect3D		m_boundRect;
	CPoint3D	m_centroid;

	CPoint3D	*m_pts;
	CPoint3D	*m_samplePts;
	CVector3D   *m_vecs;
	CVector3D   *m_sampleVecs;

friend bool	FlowClusterCopy(const FlowCluster*, FlowCluster*);
};

#endif // !defined(AFX_FLOWCLUSTER_H__B422AF88_A2DD_4C98_9EBC_479C23EF3382__INCLUDED_)
