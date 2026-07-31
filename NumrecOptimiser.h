// NumrecOptimiser.h: interface for the CNumrecOptimiser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUMRECOPTIMISER_H__2F19953D_F468_4572_90CA_E4D41688A1BC__INCLUDED_)
#define AFX_NUMRECOPTIMISER_H__2F19953D_F468_4572_90CA_E4D41688A1BC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CNumrecOptimiser  
{
public:
	CNumrecOptimiser();
	virtual ~CNumrecOptimiser();
	void amoeba(float **p, float y[], int ndim, float ftol, float (*funk)(float []), int *nfunk);
	void powell(float p[], float *xi[], int n, float ftol, int *iter, float *fret, float (*func)(float []));

protected:
	float brent(float ax, float bx, float cx, float (*f)(float), float tol, float *xmin);
	void mnbrak(float *ax, float *bx, float *cx, float *fa, float *fb, float *fc, float (*func)(float));
	void linmin(float p[], float xi[], int n, float *fret, float (*func)(float []));
	float amotry(float **p, float y[], float psum[], int ndim,float (*funk)(float []), int ihi, float fac);
};

#endif // !defined(AFX_NUMRECOPTIMISER_H__2F19953D_F468_4572_90CA_E4D41688A1BC__INCLUDED_)
