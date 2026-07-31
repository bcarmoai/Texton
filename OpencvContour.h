// OpencvContour.h: interface for the COpencvContour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPENCVCONTOUR_H__D416A5AA_BC26_4208_ABC3_3EEE366C8B03__INCLUDED_)
#define AFX_OPENCVCONTOUR_H__D416A5AA_BC26_4208_ABC3_3EEE366C8B03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class COpencvContour  
{
public:
	BOOL GetCirclePoints(int cRadius, int cX, int cY,
		CArray<CPoint, CPoint> & circle);
	BOOL fit_circle(CArray<CPoint, CPoint> & points, int &radius, int &cx, int &cy);
	BOOL FitCircle(CArray<CPoint, CPoint> & points, CArray<CPoint, CPoint> & circle,
					int width, int height, BOOL silent = FALSE, int* Cr=NULL, int* Cx=NULL, int* Cy=NULL);
	BOOL FitEllipse(CArray<CPoint, CPoint> & points, CArray<CPoint, CPoint> & ellipse,
					int width, int height);
	COpencvContour();
	virtual ~COpencvContour();

};

#endif // !defined(AFX_OPENCVCONTOUR_H__D416A5AA_BC26_4208_ABC3_3EEE366C8B03__INCLUDED_)
