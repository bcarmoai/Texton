// Vector2D.h: interface for the CVector2D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR2D_H__C9793682_3AEE_11D6_9F62_C2C2AFA7264D__INCLUDED_)
#define AFX_VECTOR2D_H__C9793682_3AEE_11D6_9F62_C2C2AFA7264D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVector2D : public CObject  
{

	DECLARE_DYNCREATE(CVector2D)

public:
	CVector2D();
	CVector2D(const CVector2D& vec) { x = vec.x, y = vec.y; }
	CVector2D(double _x, double _y) { x = _x; y = _y ;} 

	virtual ~CVector2D();

public:

	double		x;
	double		y;

	double NormSquare();
	CVector2D ProjectOrthogonal(CVector2D&);
	
	// pauline  18/07/02
	double EuclideanDist(CVector2D&);
	double EuclideanDistSquare(CVector2D&);
	//

	void SetComp(double ux,double uy);

	CVector2D product(double alpha);
	double metricsquare(CVector2D& u);
	
	CVector2D projection(CVector2D& v);

	void		Normalize();
	void		Reset();
	double	Norm();
	double	Angle();
	double	DotProduct(const CVector2D& v);

	bool		operator == (const CVector2D& v1);
	bool		operator != (const CVector2D& v1);

	CVector2D&	operator = ( const CVector2D& v1);
	
	friend	CVector2D operator * (const double multi, const CVector2D& vec);
	friend	CVector2D operator * (const CVector2D& vec, const double multi);
	friend	CVector2D operator / (const CVector2D& vec, const double div);

	//friend	CVector2D& operator = (CVector2D& v1, const CVector2D& v2);
	friend	CVector2D  operator + (const CVector2D& v1, const CVector2D& v2);
	friend	CVector2D& operator += (CVector2D& v1, const CVector2D& v2);

	friend	CVector2D operator - (const CVector2D& v1, const CVector2D& v2);

};

#endif // !defined(AFX_VECTOR2D_H__C9793682_3AEE_11D6_9F62_C2C2AFA7264D__INCLUDED_)
