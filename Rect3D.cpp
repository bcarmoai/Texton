// Rect3D.cpp: implementation of the CRect3D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "Rect3D.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRect3D::CRect3D()
{
	back = bottom = front = left = right = top = 0;
}

CRect3D::~CRect3D()
{

}

CRect3D::CRect3D(int l, int t, int r, int b, int f, int bk)
{
	if(b > 10000 || bk > 10000)
		MessageBeep(0);

	left = l;
	top = t;
	right = r;
	bottom = b;
	front = f;
	back = bk;
}

BOOL CRect3D::UnionRect(CRect3D& lpRect1, CRect3D& lpRect2)
{
	if(		lpRect1.back < 0	|| lpRect1.front < 0 || lpRect1.top < 0
		||	lpRect1.bottom < 0	|| lpRect1.left < 0  || lpRect1.right < 0
		||  lpRect1.back < lpRect1.front || lpRect1.bottom < lpRect1.top
		||	lpRect1.right < lpRect1.left 

		||	lpRect2.back < 0	|| lpRect2.front < 0 || lpRect2.top < 0
		||	lpRect2.bottom < 0	|| lpRect2.left < 0  || lpRect2.right < 0
		||  lpRect2.back < lpRect2.front || lpRect2.bottom < lpRect2.top
		||	lpRect2.right < lpRect2.left   )
		AfxMessageBox("Warning: bad CRect3D coordinates.");

	back	= lpRect1.back > lpRect2.back ? lpRect1.back : lpRect2.back;
	front	= lpRect1.front < lpRect2.front ? lpRect1.front : lpRect2.front;
	top		= lpRect1.top < lpRect2.top ? lpRect1.top : lpRect2.top;
	bottom	= lpRect1.bottom > lpRect2.bottom ? lpRect1.bottom : lpRect2.bottom;
	left	= lpRect1.left < lpRect2.left ? lpRect1.left : lpRect2.left;
	right	= lpRect1.right > lpRect2.right ? lpRect1.right : lpRect2.right;

	if(left<0 || right<0 || top<0 || bottom<0 || front<0 || back<0)
		AfxMessageBox("Error: coordinate < 0");

	return TRUE;
}

void CRect3D::InflateRect(int x, int y, int z)
{
	left -= x;
	right += x;
	top -= y;
	bottom += y;
	front -= z;
	back += z;
/*
	if(left<0 || right<0 || top<0 || bottom<0 || front<0 || back<0)
		AfxMessageBox("Error: coordinate < 0");
*/
	
}

BOOL CRect3D::IntersectRect(CRect3D& lpRect1, CRect3D lpRect2)
{
	// only called by FlowAbstraction:
	//  boundRect.IntersectRect(boundRect, CRect3D(0, 0, m_vecSizeX, m_vecSizeY, 0, m_vecSizeZ));
	//                                             l  t       r           b      f       bk

	if(		/*lpRect1.back < 0	|| lpRect1.front < 0 || lpRect1.top < 0
		||	lpRect1.bottom < 0	|| lpRect1.left < 0  || lpRect1.right < 0
		||*/  lpRect1.back < lpRect1.front || lpRect1.bottom < lpRect1.top
		||	lpRect1.right < lpRect1.left 

		/*||	lpRect2.back < 0	|| lpRect2.front < 0 || lpRect2.top != 0
		||	lpRect2.bottom < 0	|| lpRect2.left != 0  || lpRect2.right != 0 */
		||  lpRect2.back < lpRect2.front || lpRect2.bottom < lpRect2.top
		||	lpRect2.right < lpRect2.left   )
		AfxMessageBox("Warning: bad CRect3D coordinates.");

	back	= lpRect1.back > lpRect2.back ? lpRect2.back : lpRect1.back;
	front	= lpRect1.front < lpRect2.front ? lpRect2.front : lpRect1.front;
	top		= lpRect1.top < lpRect2.top ? lpRect2.top : lpRect1.top;
	bottom	= lpRect1.bottom > lpRect2.bottom ? lpRect2.bottom : lpRect1.bottom;
	left	= lpRect1.left < lpRect2.left ? lpRect2.left : lpRect1.left;
	right	= lpRect1.right > lpRect2.right ? lpRect2.right : lpRect1.right;

	if(left<0 || right<0 || top<0 || bottom<0 || front<0 || back<0)
		AfxMessageBox("Error: coordinate < 0");

	return TRUE;
}

BOOL CRect3D::PtInRect(CPoint3D &point)
{
	if(point.x >= left && point.x < right && point.y >= top && point.y < bottom
		&& point.z >= front && point.z < back)
		return TRUE;
	else
		return FALSE;

}
