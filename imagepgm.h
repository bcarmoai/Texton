// ImagePgm.h: interface for the CImagePgm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGEPGM_H__450B3239_42CC_4EC3_AD2E_5888D7AB5B64__INCLUDED_)
#define AFX_IMAGEPGM_H__450B3239_42CC_4EC3_AD2E_5888D7AB5B64__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CImagePgm  
{
public:
	void SaveImagePgm(char* filename, int scale =1);
	CImagePgm();
	virtual ~CImagePgm();
	int LoadPgm(CString fileName);
	int imgWidth;
	int imgHeight;
	int* imgData;

protected:
	long NextCharPos(FILE* oFile);

};

#endif // !defined(AFX_IMAGEPGM_H__450B3239_42CC_4EC3_AD2E_5888D7AB5B64__INCLUDED_)
