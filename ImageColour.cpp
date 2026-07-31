// ImageColour.cpp: implementation of the CImageColour class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "ImageColour.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImageColour::CImageColour()
{
	imgData = NULL;
	imgHeight = 0;
	imgWidth = 0;
	imgSlices = 0;
	mode3D = FALSE;
}

CImageColour::~CImageColour()
{
	if(imgData != NULL)
		delete [] imgData;

}

void CImageColour::AllocateData(int width, int height)
{
	if(imgData != NULL)
		delete [] imgData;

	imgData = new CImageRGB[width*height];

	if(imgData == NULL)
	{
		AfxMessageBox("Unexpected error: could not allocate colour image.");
	}

	imgWidth = width;
	imgHeight = height;
	mode3D = FALSE;
}

void CImageColour::AllocateData3D(int width, int height, int slices)
{
	if(imgData != NULL)
		delete [] imgData;

	imgData = new CImageRGB[width*height*slices];

	if(imgData == NULL)
	{
		AfxMessageBox("Unexpected error: could not allocate colour image.");
	}

	imgWidth = width;
	imgHeight = height;
	imgSlices = slices;
	mode3D = TRUE;
}
