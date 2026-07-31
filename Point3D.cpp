// Point3D.cpp: implementation of the CPoint3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "Point3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPoint3D::CPoint3D()
{

}

CPoint3D::~CPoint3D()
{

}

CPoint3D::CPoint3D(int u, int v, int w)
{
	x=u;
	y=v;
	z=w;
}

CPoint3D::operator=(const CPoint3D &toCopy)
{
	x=toCopy.x;
	y=toCopy.y;
	z=toCopy.z;
}

BOOL CPoint3D::operator==(const CPoint3D &toCheck)
{
	if( x == toCheck.x && y == toCheck.y && z == toCheck.z)
		return TRUE;
	else
		return FALSE;
}
