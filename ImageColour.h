// ImageColour.h: interface for the CImageColour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGECOLOUR_H__B75E2A34_C33C_49A1_AA96_1854A61EB4B8__INCLUDED_)
#define AFX_IMAGECOLOUR_H__B75E2A34_C33C_49A1_AA96_1854A61EB4B8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ImageRGB.h"

class CImageColour  
{
public:
	int imgSlices;
	void AllocateData3D(int width, int height, int slices);
	void AllocateData(int width, int height);
	CImageRGB* imgData;
	int imgHeight;
	int imgWidth;
	CImageColour();
	~CImageColour();
	BOOL mode3D;

};

#endif // !defined(AFX_IMAGECOLOUR_H__B75E2A34_C33C_49A1_AA96_1854A61EB4B8__INCLUDED_)
