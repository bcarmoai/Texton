// Vector3D.cpp: implementation of the CVector3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include "Vector3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CVector3D::~CVector3D()
{

}



//////////////////////////////////////////////////////////////////////
// member functions
//////////////////////////////////////////////////////////////////////

void	CVector3D::Reset(){
	x = y = z = 0;
}

void	CVector3D::Normalize()
{
	if ( x == 0 && y == 0 && z == 0) 
		return;

	double norm = this->Norm();
	x = x / norm;
	y = y / norm;
	z = z / norm;

	return;
}

double	CVector3D::Norm()
{
	return (sqrt(x * x  + y * y + z * z));
}


double CVector3D::NormSquare()
{
		return( x * x + y * y + z * z);
}


// Calculate the Euclidean distance between two vector
//
double CVector3D::EuclideanDist(CVector3D &v)
{
	double dx, dy, dz;
	
	dx = x - v.x;
	dy = y - v.y;
	dz = z - v.z;

	return ( sqrt( dx * dx + dy * dy + dz * dz));
}

  
//
// pauline  18/07/02
// Calculate the Squared Euclidean distance between two vector
//
double CVector3D::EuclideanDistSquare(CVector3D &v)
{
	double dx, dy, dz;
	
	dx = x - v.x;
	dy = y - v.y;
	dz = z - v.z;

	return (( dx * dx + dy * dy + dz * dz));
}



CVector3D&	CVector3D::operator =(const CVector3D& v1)
{
	x = v1.x;
	y = v1.y;
	z = v1.z;

	return *this;
}

bool	CVector3D::operator ==(const CVector3D& v1)
{
	return (x == v1.x && y == v1.y && z == v1.z) ;
}

bool	CVector3D::operator !=(const CVector3D& v1)
{
	return (x != v1.x || y != v1.y || z != v1.z) ;
}

double	CVector3D::DotProduct(const CVector3D& v1)
{
	return ( x * v1.x + y * v1.y + z * v1.z);
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
// friend interface

/*
CVector3D& operator =(CVector3D& v1, const CVector3D& v2)
{
	v1.x = v2.x;
	v1.y = v2.y;

	return v1;
}

*/
CVector3D operator * (const double multi, const CVector3D& vec)
{
	CVector3D	result;

	result.x = vec.x * multi;
	result.y = vec.y * multi;
	result.z = vec.z * multi;
	
	return result;
}



CVector3D operator * (const CVector3D& vec, const double multi)
{
	CVector3D result;

	result.x = vec.x * multi;
	result.y = vec.y * multi;
	result.z = vec.z * multi;
	
	return result;
}


CVector3D operator / (const CVector3D& vec, const double div)
{
	CVector3D result;

	result.x = vec.x / div;
	result.y = vec.y / div;
	result.z = vec.z / div;
	
	return result;
}


CVector3D operator + (const CVector3D& v1, const CVector3D& v2)
{
	CVector3D	result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
}

CVector3D& operator += (CVector3D& v1, const CVector3D& v2)
{
	v1.x = v1.x + v2.x;
	v1.y = v1.y + v2.y;
	v1.z = v1.z + v2.z;

	return v1;
}


CVector3D operator - (const CVector3D& v1, const CVector3D& v2)
{
	CVector3D	result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
}

//  end friend function
//////////////////////////////////////////////////
//////////////////////////////////////////////////



void CVector3D::SetComp(double vx, double vy, double vz)
{
	x = vx, y = vy, z = vz;
}
