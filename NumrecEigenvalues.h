// NumrecEigenvalues.h: interface for the CNumrecEigenvalues class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NUMRECEIGENVALUES_H__43C6A6FE_D674_41E2_A521_27DC6C144775__INCLUDED_)
#define AFX_NUMRECEIGENVALUES_H__43C6A6FE_D674_41E2_A521_27DC6C144775__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNumrecEigenvalues  
{
public:
	int gaussj(float **a, int n, float **b, int m);
	void ComputeEigenvalues(float** a, float* wr, float* wi, int n);
	CNumrecEigenvalues();
	virtual ~CNumrecEigenvalues();
private:
	void balanc(float **a, int n);  // first step to balance the matrix
	void elmhes(float **a, int n);  // second step to turn matrix into Hessenberg form
	void hqr(float **a, int n, float wr[], float wi[]);  // third & final step to find eigenvalues
};

#endif // !defined(AFX_NUMRECEIGENVALUES_H__43C6A6FE_D674_41E2_A521_27DC6C144775__INCLUDED_)
