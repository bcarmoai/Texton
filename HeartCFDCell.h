// HeartCFDCell.h: interface for the CHeartCFDCell class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HEARTCFDCELL_H__878B08E5_7E98_45E6_9426_1CE745CE2B69__INCLUDED_)
#define AFX_HEARTCFDCELL_H__878B08E5_7E98_45E6_9426_1CE745CE2B69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HeartCFDVertex.h"

class CHeartCFDCell  
{
public:
	float GetTotalPointDistance(CHeartCFDVertex* vertex_array, float x, float y, float z);
	float GetCellPoint(float x, float y, float z, float& velx, float& vely, float& velz);
	void CreateInterpolatedData(CHeartCFDVertex* vertex_array, int cfdHeight,
						float* u_voxels, float* v_voxels, float* w_voxels, int* vPointCount);
	CHeartCFDCell();
	virtual ~CHeartCFDCell();
	int vertex_points[9]; // see log 6/5/03; 8 vertices +1 because of 1-based indexing
	void SaveVtkCell();

protected:
	int FloatToInt(float fnum);
	void AddPointData(int point_no, int cfdHeight, int* vPointCount, 
		float px, float py, float pz, float pvelx, float pvely, float pvelz, 
		float *u_voxels, float *v_voxels, float *w_voxels);
	BOOL TestNodeT(float t1, float t2, float t3);
	int interpPointCount;
	CHeartCFDVertex* interpPointData;

};

#endif // !defined(AFX_HEARTCFDCELL_H__878B08E5_7E98_45E6_9426_1CE745CE2B69__INCLUDED_)
