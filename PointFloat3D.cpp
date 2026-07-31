// PointFloat3D.cpp: implementation of the CPointFloat3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "PointFloat3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPointFloat3D::CPointFloat3D()
{

}

CPointFloat3D::~CPointFloat3D()
{

}

CPointFloat3D::CPointFloat3D(float u, float v, float w)
{
	x=u;
	y=v;
	z=w;
}

CPointFloat3D::operator=(const CPointFloat3D &toCopy)
{
	x=toCopy.x;
	y=toCopy.y;
	z=toCopy.z;
}

BOOL CPointFloat3D::operator==(const CPointFloat3D &toCheck)
{
	if( x == toCheck.x && y == toCheck.y && z == toCheck.z)
		return TRUE;
	else
		return FALSE;
}
