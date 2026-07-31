// Plane.h: interface for the CPlane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANE_H__48535282_6FF2_438F_B1AF_BC99F3DB7DAC__INCLUDED_)
#define AFX_PLANE_H__48535282_6FF2_438F_B1AF_BC99F3DB7DAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Vector3D.h"
#include "Point3D.h"	// Added by ClassView

class CPlane  
{
public:
	operator=(const CPlane& toCopy);
	CPlane();
	CPlane(CVector3D &normal, double dist);

	virtual ~CPlane();

	CVector3D		vecNormal;
	double			distFromOri;

public:
	void SetValuesFromFileData(BOOL b3D, float av_vx, float av_vy, float av_vz, 
		float** matrixA, float* cv0, int cx, int cy, int cz);
	float* GetV0();
	CPoint3D GetCentroid();
	float** GetMatrixA();
	CVector3D GetAverageV();
	CPoint3D GetAverageX();
	int FloatToInt(float fnum);
	CVector3D GetFitVel(int x, int y, int z);
	int			SolveForZ(double x, double y, double *z);
	int			LeastSquareFitting(BOOL b3D, CPoint3D *pts, CVector3D *vectors, int nPts);
	double	SqDistFromPt(CPoint3D pt, CVector3D vc);


private:
	bool		PtOnPlane(CPoint3D &pt, CVector3D &vc);
	double	DistFromPt(CPoint3D pt, CVector3D vc);
	double	CalDeterminant(double matrixBuf[]);
	int			SolveEquations(double matrixBufA[], double matrixB[], CVector3D*);
	float m_lmsAverageV[3];
	float **matrix_a;

protected:
	BOOL bMatrixaValid;
	BOOL m_b3D;
	CPoint3D centroid;
	float v0[3];
};

#endif // !defined(AFX_PLANE_H__48535282_6FF2_438F_B1AF_BC99F3DB7DAC__INCLUDED_)
