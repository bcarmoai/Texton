// PointFloat3D.h: interface for the CPointFloat3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINTFLOAT3D_H__0F4B5606_08CE_4F9D_B4F9_72AD617524F6__INCLUDED_)
#define AFX_POINTFLOAT3D_H__0F4B5606_08CE_4F9D_B4F9_72AD617524F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPointFloat3D  
{
public:
	float z;
	float y;
	float x;
	CPointFloat3D(float u, float v, float w);
	CPointFloat3D();
	virtual ~CPointFloat3D();
	operator=(const CPointFloat3D &toCopy);
	BOOL operator==(const CPointFloat3D &toCheck);

};

#endif // !defined(AFX_POINTFLOAT3D_H__0F4B5606_08CE_4F9D_B4F9_72AD617524F6__INCLUDED_)
