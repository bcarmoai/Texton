// Rect3D.h: interface for the CRect3D class.
//
//////////////////////////////////////////////////////////////////////

#include "Point3D.h"

#if !defined(AFX_RECT3D_H__7BCD48F4_4F91_4DB6_9412_C77DE46A40A3__INCLUDED_)
#define AFX_RECT3D_H__7BCD48F4_4F91_4DB6_9412_C77DE46A40A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRect3D  
{
public:
	BOOL PtInRect( CPoint3D& point );
	BOOL IntersectRect( CRect3D& lpRect1, CRect3D lpRect2 );
	void InflateRect(int x, int y, int z);
	BOOL UnionRect(CRect3D& lpRect1, CRect3D& lpRect2);
	CRect3D(int l, int t, int r, int b, int f, int bk);
	CRect3D();
	virtual ~CRect3D();
	operator=(const CRect3D& lpRect){bottom=lpRect.bottom;left=lpRect.left;top=lpRect.top;
		right=lpRect.right;back=lpRect.back;front=lpRect.front;
		if(left<0 || right<0 || top<0 || bottom<0 || front<0 || back<0)
		AfxMessageBox("Error: coordinate < 0");};
	int GetLeft() {return left;};
	int GetTop() {return top;};
	int GetRight() {return right;};
	int GetBottom() {return bottom;};
	int GetFront() {return front;};
	int GetBack() {return back;};

protected:
	int left;
	int top;
	int right;
	int bottom;
	int front;
	int back;

};

#endif // !defined(AFX_RECT3D_H__7BCD48F4_4F91_4DB6_9412_C77DE46A40A3__INCLUDED_)
