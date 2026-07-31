// Point3D.h: interface for the CPoint3D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT3D_H__CA598812_BFF9_4529_A272_4E057100BCC6__INCLUDED_)
#define AFX_POINT3D_H__CA598812_BFF9_4529_A272_4E057100BCC6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPoint3D  
{
public:
	CPoint3D(int u, int v, int w);
	int z;
	int y;
	int x;
	CPoint3D();
	virtual ~CPoint3D();
	operator=(const CPoint3D &toCopy);
	BOOL operator==(const CPoint3D &toCheck);

};

#endif // !defined(AFX_POINT3D_H__CA598812_BFF9_4529_A272_4E057100BCC6__INCLUDED_)
