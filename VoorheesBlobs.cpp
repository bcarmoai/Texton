// VoorheesBlobs.cpp: implementation of the CVoorheesBlobs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "VoorheesBlobs.h"
#include "WrapperLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVoorheesBlobs::CVoorheesBlobs()
{
	imgLoG = NULL;
}

CVoorheesBlobs::~CVoorheesBlobs()
{

}

void CVoorheesBlobs::LoG(int * input, int * output, int width, int height, float thres)
{
	CWrapperLoG acLoG;

	acLoG.DoMarrHil(input, output, width, height, thres);
	
	imgLoG = new int[width*height];
	int x, y;
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			imgLoG[width*y+x] = output[width*y+x];

}

void CVoorheesBlobs::Reset()
{
	if(imgLoG == NULL)
		delete imgLoG;
	imgLoG = NULL;
	
}

void CVoorheesBlobs::ThresLoG(int * output, int width, int height, int thres)
{
	int x, y;
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			if(imgLoG[width*y+x] > thres)
				output[width*y+x] = 255;
			else
				output[width*y+x] = imgLoG[width*y+x];

		}
		
}

void CVoorheesBlobs::GaussBlur(int * input, int * output, int width, int height, float sigma)
{
	CWrapperLoG acLoG;

	acLoG.GaussBlur(input, output, width, height, sigma);

}
