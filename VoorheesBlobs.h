// VoorheesBlobs.h: interface for the CVoorheesBlobs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VOORHEESBLOBS_H__B3AB32C7_1090_493C_B881_5C8C1C8D1476__INCLUDED_)
#define AFX_VOORHEESBLOBS_H__B3AB32C7_1090_493C_B881_5C8C1C8D1476__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CVoorheesBlobs  
{
public:
	void GaussBlur(int * input, int * output, int width, int height, float sigma);
	void ThresLoG(int* output, int width, int height, int thres);
	void Reset();
	void LoG(int* input, int* output, int width, int height, float thres);
	CVoorheesBlobs();
	virtual ~CVoorheesBlobs();

protected:
	int* imgLoG;
};

#endif // !defined(AFX_VOORHEESBLOBS_H__B3AB32C7_1090_493C_B881_5C8C1C8D1476__INCLUDED_)
