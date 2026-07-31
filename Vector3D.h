// Vector3D.h: interface for the CVector3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR3D_H__57128D56_55F5_4908_97BA_9E982F0D71F9__INCLUDED_)
#define AFX_VECTOR3D_H__57128D56_55F5_4908_97BA_9E982F0D71F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Point3D.h"

class CVector3D  
{
public:
	CVector3D() { x = y = z = 0; };
	CVector3D(const CPoint3D& pt) { x = pt.x, y = pt.y, z = pt.z ; }
	CVector3D(const CVector3D& vec) { x = vec.x, y = vec.y, z = vec.z ; }
	CVector3D(double _x, double _y, double _z) { x = _x; y = _y , z = _z;} 

	virtual ~CVector3D();

public:
	void SetComp(double vx, double vy, double vz);
	double		x;
	double		y;
	double		z;

	double	NormSquare();
	double	Norm();
	void		Normalize();
	void		Reset();

	double EuclideanDist(CVector3D&);
	double EuclideanDistSquare(CVector3D&);

	double	DotProduct(const CVector3D& v);

	bool		operator == (const CVector3D& v1);
	bool		operator != (const CVector3D& v1);

	CVector3D&	operator = ( const CVector3D& v1);
	
	friend	CVector3D operator * (const double multi, const CVector3D& vec);
	friend	CVector3D operator * (const CVector3D& vec, const double multi);
	friend	CVector3D operator / (const CVector3D& vec, const double div);

	friend	CVector3D  operator + (const CVector3D& v1, const CVector3D& v2);
	friend	CVector3D& operator += (CVector3D& v1, const CVector3D& v2);

	friend	CVector3D operator - (const CVector3D& v1, const CVector3D& v2);
};

#endif // !defined(AFX_VECTOR3D_H__57128D56_55F5_4908_97BA_9E982F0D71F9__INCLUDED_)
