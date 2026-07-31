// DlgStrmOriginalData.cpp: implementation of the CDlgStrmOriginalData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "DlgStrmOriginalData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgStrmOriginalData::CDlgStrmOriginalData()
{
	mri = u = v = w = NULL;
	img_width = img_height = 0;
}

CDlgStrmOriginalData::~CDlgStrmOriginalData()
{
	if(mri!=NULL)
		delete mri;
	if(u!=NULL)
		delete u;
	if(v!=NULL)
		delete v;
	if(w!=NULL)
		delete w;
}

void CDlgStrmOriginalData::SetData(int width, int height, float * amri, float * au, float * av, float * aw)
{
	if(width < 0 || height < 0 || amri == NULL || au!=NULL || aw!=NULL || av!=NULL)
	{
		AfxMessageBox("Unexpected error: SetData() called with invalid arguments.");
	}

	img_width = width;
	img_height = height;

	if(mri!=NULL)
		delete mri;
	if(u!=NULL)
		delete u;
	if(v!=NULL)
		delete v;
	if(w!=NULL)
		delete w;	

	mri = new float[width*height];
	u = new float[width*height];
	v = new float[width*height];
	w = new float[width*height];

	int x, y;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			mri[img_width*y+x]=amri[img_width*y+x];
			u[img_width*y+x]=au[img_width*y+x];
			v[img_width*y+x]=av[img_width*y+x];
			w[img_width*y+x]=aw[img_width*y+x];

		}

}

void CDlgStrmOriginalData::GetData(int & width, int & height, float * amri, float * au, float * av, float * aw)
{
	img_width = width;
	img_height = height;

	int x, y;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			amri[img_width*y+x]=mri[img_width*y+x];
			au[img_width*y+x]=u[img_width*y+x];
			av[img_width*y+x]=v[img_width*y+x];
			aw[img_width*y+x]=w[img_width*y+x];
		}

}
