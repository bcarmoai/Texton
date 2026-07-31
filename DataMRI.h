// DataMRI.h: interface for the CDataMRI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAMRI_H__52E67998_764E_4137_8016_7FD8B09874B9__INCLUDED_)
#define AFX_DATAMRI_H__52E67998_764E_4137_8016_7FD8B09874B9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CDataMRI  
{
public:
	void SaveDataSur(int idx, int fn_slice);
//	void Zoom(CPoint upperLeft, CPoint lowerRight, int dest_size);
	void SetData(int width, int height, short* amri, short* au, short* av, short* aw);
	int img_height;
	int img_width;
	void Reset();
	void LoadData(CString filename);
	short* w;
	short* v;
	short* u;
	short* mr;
	CDataMRI();
	virtual ~CDataMRI();
	void ZoomBlurAll();

protected:
};

#endif // !defined(AFX_DATAMRI_H__52E67998_764E_4137_8016_7FD8B09874B9__INCLUDED_)
