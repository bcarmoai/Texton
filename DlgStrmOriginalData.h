// DlgStrmOriginalData.h: interface for the CDlgStrmOriginalData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGSTRMORIGINALDATA_H__34ABF648_7C01_4154_86D9_19241823C3C4__INCLUDED_)
#define AFX_DLGSTRMORIGINALDATA_H__34ABF648_7C01_4154_86D9_19241823C3C4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDlgStrmOriginalData  
{
public:
	void GetData(int& width, int& height, float* amri, float* au, float* av, float* aw);
	void SetData(int width, int height, float* amri, float* au, float* av, float* aw);
	CDlgStrmOriginalData();
	virtual ~CDlgStrmOriginalData();

protected:
	float* mri;
	float* w;
	float* v;
	float* u;
	int img_width;
	int img_height;
};

#endif // !defined(AFX_DLGSTRMORIGINALDATA_H__34ABF648_7C01_4154_86D9_19241823C3C4__INCLUDED_)
