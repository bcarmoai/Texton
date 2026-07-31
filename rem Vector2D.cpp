// Vector2D.cpp: implementation of the CVector2D class.
//
//////////////////////////////////////////////////////////////////////

#include <afx.h>
#include "stdafx.h"

#include <math.h>
#include "Vector2D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE( CVector2D, CObject )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVector2D::CVector2D()
{
	x = y = 0;
}

CVector2D::~CVector2D()
{

}


//////////////////////////////////////////////////////////////////////
// member functions
//////////////////////////////////////////////////////////////////////

void	CVector2D::Reset(){
	x = y = 0;
}

void	CVector2D::Normalize()
{
	if ( x ==0 && y ==0) 
		return;

	double mag = sqrt(x * x  + y * y);
	x = x / mag;
	y = y / mag;
	return;
}

double	CVector2D::Norm()
{
	double	norm;

	norm = sqrt(x * x  + y * y);
	return (norm);
}


double	CVector2D::Angle()
{
	if ( fabs(x) < 1.0e-6 && fabs(y) < 1.0e-6)
		return (0);
	else 
		return ( atan2(y, x));
}


CVector2D&	CVector2D::operator =(const CVector2D& v1)
{
	x = v1.x;
	y = v1.y;

	return *this;
}

bool	CVector2D::operator ==(const CVector2D& v1)
{
	return (x == v1.x && y == v1.y) ;
}

bool	CVector2D::operator !=(const CVector2D& v1)
{
	return (x != v1.x || y != v1.y) ;
}

double	CVector2D::DotProduct(const CVector2D& v1)
{
	return ( x * v1.x + y * v1.y);
}


//////////////////////////////////////////////////
//////////////////////////////////////////////////
//////////////////////////////////////////////////
// friend interface

/*
CVector2D& operator =(CVector2D& v1, const CVector2D& v2)
{
	v1.x = v2.x;
	v1.y = v2.y;

	return v1;
}

*/
CVector2D operator * (const double multi, const CVector2D& vec)
{
	CVector2D	result;

	result.x = vec.x * multi;
	result.y = vec.y * multi;
	
	return result;
}



CVector2D operator * (const CVector2D& vec, const double multi)
{
	CVector2D result;

	result.x = vec.x * multi;
	result.y = vec.y * multi;
	
	return result;
}


CVector2D operator / (const CVector2D& vec, const double div)
{
	CVector2D result;

	result.x = vec.x / div;
	result.y = vec.y / div;
	
	return result;
}


CVector2D operator + (const CVector2D& v1, const CVector2D& v2)
{
	CVector2D	result;

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;

	return result;
}

CVector2D& operator += (CVector2D& v1, const CVector2D& v2)
{
	v1.x = v1.x + v2.x;
	v1.y = v1.y + v2.y;

	return v1;
}


CVector2D operator - (const CVector2D& v1, const CVector2D& v2)
{
	CVector2D	result;

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;

	return result;
}

//  friend function
//////////////////////////////////////////////////
//////////////////////////////////////////////////



CVector2D CVector2D::projection(CVector2D& v)
{
	CVector2D proj;
	CVector2D u;

	u.x = v.x/(v.Norm() * v.Norm());
	u.y = v.y/(v.Norm() * v.Norm());
	//u.setcomp(u.x, u.y);  // pauline
	proj = *this - this->DotProduct(v) * u;
	
	return proj;
}

void CVector2D::SetComp(double ux, double uy)
{
	this->x = ux;
	this->y = uy;

}



double CVector2D::metricsquare(CVector2D &u)
{
	double distance;
	distance = (this->x - u.x)*(this->x - u.x) + (this->y - u.y)*(this->y - u.y);
	return distance;
}



CVector2D CVector2D::product(double alpha)
{
	CVector2D u;
	u.x = alpha * this->x;
	u.y = alpha * this->y;
	// pauline
	// u.setcomp(u.x, u.y);
	return u;
}


//  
// pauline  18/07/02
// Calculate the Euclidean distance between two vector
//
double CVector2D::EuclideanDist(CVector2D &v)
{
	double dx, dy;
	
	dx = x - v.x;
	dy = y - v.y;

	return ( sqrt( dx * dx + dy * dy));
}

  
//
// pauline  18/07/02
// Calculate the Squared Euclidean distance between two vector
//
double CVector2D::EuclideanDistSquare(CVector2D &v)
{
	double dx, dy;
	
	dx = x - v.x;
	dy = y - v.y;

	return (( dx * dx + dy * dy));
}

CVector2D CVector2D::ProjectOrthogonal(CVector2D &v)
{
	// this function return a zero Vector if Norm(v)==0
	// return the projection onto the plane orthogonal to vector v

	CVector2D proj;
	double		vNormSq;

	vNormSq = v.NormSquare();
	
	if (vNormSq > 0)
		proj = *this - v * (this->DotProduct(v) / vNormSq);
	
	return proj;
}

double CVector2D::NormSquare()
{
		return( x * x + y * y);
}
