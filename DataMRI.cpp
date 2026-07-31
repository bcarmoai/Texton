// DataMRI.cpp: implementation of the CDataMRI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "DataMRI.h"
#include <stdlib.h>
//#include <ipl.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataMRI::CDataMRI()
{
	mr = u = v = w = NULL;
}

CDataMRI::~CDataMRI()
{

	if(mr != NULL)
		delete mr;

	if(u != NULL)
		delete u;
	
	if(v != NULL)
		delete v;
	
	if(w != NULL)
		delete w;

}

void CDataMRI::LoadData(CString filename)
{
	CString mrFilename;
	CString uFilename;
	CString vFilename;
	CString wFilename;

	// check for good filename
	mrFilename = filename;
	int slash = mrFilename.ReverseFind('\\');
	if(mrFilename.GetLength()-slash-1 != 9)  // "mr001.sur" = 9 chars
	{
		AfxMessageBox("Please choose a filename of type 'mrXXX.sur'.");
		return;
	}

	// generate other paths
	uFilename = mrFilename.Left(slash+1) + "u" + mrFilename.Right(7);
	vFilename = mrFilename.Left(slash+1) + "v" + mrFilename.Right(7);
	wFilename = mrFilename.Left(slash+1) + "w" + mrFilename.Right(7);

	// allocate memory
	if(mr != NULL)
		delete mr;
	if(u != NULL)
		delete u;
	if(v != NULL)
		delete v;
	if(w != NULL)
		delete w;
	mr = new short[256*256];
	u = new short[256*256];
	v = new short[256*256];
	w = new short[256*256];

	// read data from files
	const int dataLength = 256*256*2;
	CFile fMR(mrFilename, CFile::modeRead);
	fMR.Seek(512, CFile::begin );
	if(fMR.Read(mr,dataLength) != dataLength)
		AfxMessageBox("Warning: MR data shorter than 256*256*2 bytes.");
	fMR.Close();

	CFile fU(uFilename, CFile::modeRead);
	fU.Seek(512, CFile::begin );
	if(fU.Read(u,dataLength) != dataLength)
		AfxMessageBox("Warning: U data shorter than 256*256*2 bytes.");
	fU.Close();
	
	CFile fV(vFilename, CFile::modeRead);
	fV.Seek(512, CFile::begin );
	if(fV.Read(v,dataLength) != dataLength)
		AfxMessageBox("Warning: V data shorter than 256*256*2 bytes.");
	fV.Close();
/*	
	CFile fW(wFilename, CFile::modeRead);
	fW.Seek(512, CFile::begin );
	if(fW.Read(w,dataLength) != dataLength)
		AfxMessageBox("Warning: W data shorter than 256*256*2 bytes.");
	fW.Close();
*/
	img_height = img_width = 256;


}

void CDataMRI::Reset()
{
	if(mr != NULL)
		delete [] mr;
	mr=NULL;
	if(u != NULL)
		delete [] u;
	u=NULL;
	if(v != NULL)
		delete [] v;
	v=NULL;
	if(w != NULL)
		delete [] w;
	w=NULL;
	img_width = img_height = 0;
}


void CDataMRI::SetData(int width, int height, short * amri, short * au, short * av, short * aw)
{
	if(width < 0 || height < 0 || amri==NULL || au==NULL || aw==NULL || av==NULL)
	{
		AfxMessageBox("Unexpected error: SetData() called with invalid arguments.");
		return;
	}

	Reset();

	img_width = width;
	img_height = height;

	mr = new short[width*height];
	u = new short[width*height];
	v = new short[width*height];
	w = new short[width*height];

	int x, y;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			mr[img_width*y+x]=amri[img_width*y+x];
			u[img_width*y+x]=au[img_width*y+x];
			v[img_width*y+x]=av[img_width*y+x];
			w[img_width*y+x]=aw[img_width*y+x];

		}
}
/*
void CDataMRI::Zoom(CPoint upperLeft, CPoint lowerRight, int dest_size)
{
	short buffer1;
	short buffer2[4];
	int x, y;
	int dest_width, dest_height;
	dest_width = dest_height = dest_size;

	IplROI* roi = iplCreateROI(0, upperLeft.x, upperLeft.y, 
		lowerRight.x-upperLeft.x, lowerRight.y-upperLeft.y);

	// create iplsource image
	IplImage *iplSource;
	iplSource = iplCreateImageHeader(1,0,IPL_DEPTH_16U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		img_width, img_height, roi, NULL, NULL, NULL );
	if( !iplSource ) return;
	iplAllocateImage( iplSource, 0, 0 );
	if( !iplSource->imageData ) return;

	// create ipl destination image
	IplImage *iplDest;
	iplDest = iplCreateImageHeader(1,0,IPL_DEPTH_16U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		dest_width, dest_height, NULL, NULL, NULL, NULL );
	if( !iplDest ) return;
	iplAllocateImage( iplDest, 0, 0 );
	if( !iplDest->imageData ) return;

	//////////////////////////////////////////////////////
	///  MRI data zoom
	//////////////////////////////////////////////////////
	
	// copy texture to source
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			buffer1 = mr[img_width*y+x];
			iplPutPixel(iplSource, x, y, (void*)&buffer1);
		}
	
	// apply filter
	iplZoomFit(iplSource, iplDest, IPL_INTER_NN);
	
	delete [] mr;
	mr = new short[dest_width*dest_height];

	// copy image to array
	for(x=0;x<dest_width;x++)
		for(y=0;y<dest_height;y++)
		{
			iplGetPixel(iplDest, x, y, &buffer2);
			mr[dest_width*y+x] = buffer2[0];
		}

	//////////////////////////////////////////////////////
	///  U data zoom
	//////////////////////////////////////////////////////

	// copy texture to source
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			buffer1 = u[img_width*y+x];
			iplPutPixel(iplSource, x, y, (void*)&buffer1);
		}
	
	// apply filter
	iplZoomFit(iplSource, iplDest, IPL_INTER_NN);
	
	delete [] u;
	u = new short[dest_width*dest_height];

	// copy image to array
	for(x=0;x<dest_width;x++)
		for(y=0;y<dest_height;y++)
		{
			iplGetPixel(iplDest, x, y, &buffer2);
			u[dest_width*y+x] = buffer2[0];
		}

	//////////////////////////////////////////////////////
	///  V data zoom
	//////////////////////////////////////////////////////

	// copy texture to source
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			buffer1 = v[img_width*y+x];
			iplPutPixel(iplSource, x, y, (void*)&buffer1);
		}
	
	// apply filter
	iplZoomFit(iplSource, iplDest, IPL_INTER_NN);

	delete [] v;
	v = new short[dest_width*dest_height];
	
	// copy image to array
	for(x=0;x<dest_width;x++)
		for(y=0;y<dest_height;y++)
		{
			iplGetPixel(iplDest, x, y, &buffer2);
			v[dest_width*y+x] = buffer2[0];
		}


	//////////////////////////////////////////////////////
	///  W data zoom
	//////////////////////////////////////////////////////

	// copy texture to source
	for(x=0;x<img_width;x++)
		for(y=0;y<img_height;y++)
		{
			buffer1 = w[img_width*y+x];
			iplPutPixel(iplSource, x, y, (void*)&buffer1);
		}
	
	// apply filter
	iplZoomFit(iplSource, iplDest, IPL_INTER_NN);
	
	delete [] w;
	w = new short[dest_width*dest_height];

	// copy image to array
	for(x=0;x<dest_width;x++)
		for(y=0;y<dest_height;y++)
		{
			iplGetPixel(iplDest, x, y, &buffer2);
			w[dest_width*y+x] = buffer2[0];
		}

	iplDeallocateImage(iplSource);
	iplDeallocateImage(iplDest);

	img_height = dest_height;
	img_width = dest_width;

}
*/

void CDataMRI::ZoomBlurAll()
{
	AfxMessageBox("Obsolete function called.");
/*	int x, y;
	double temp;
	short* dmy_mr = new short[img_width*img_height];
	short* dmy_u = new short[img_width*img_height];
	short* dmy_v = new short[img_width*img_height];
	short* dmy_w = new short[img_width*img_height];

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			dmy_mr[img_width*y+x] = mr[img_width*y+x];
			dmy_u[img_width*y+x] = u[img_width*y+x];
			dmy_v[img_width*y+x] = v[img_width*y+x];
			dmy_w[img_width*y+x] = w[img_width*y+x];
		}

	for(x=1; x<img_width-1; x++)
		for(y=1; y<img_height-1; y++)
		{
			temp = mr[img_width*y+x];
			temp = temp + mr[img_width*y+(x-1)];
			temp = temp + mr[img_width*y+(x+1)];
			temp = temp + mr[img_width*(y+1)+(x-1)];
			temp = temp + mr[img_width*(y+1)+(x+1)];
			temp = temp + mr[img_width*(y-1)+(x-1)];
			temp = temp + mr[img_width*(y-1)+(x+1)];
			temp = temp + mr[img_width*(y+1)+x];
			temp = temp + mr[img_width*(y-1)+x];
			dmy_mr[img_width*y+x]=temp/9.0;

			temp = u[img_width*y+x];
			temp = temp + u[img_width*y+(x-1)];
			temp = temp + u[img_width*y+(x+1)];
			temp = temp + u[img_width*(y+1)+(x-1)];
			temp = temp + u[img_width*(y+1)+(x+1)];
			temp = temp + u[img_width*(y-1)+(x-1)];
			temp = temp + u[img_width*(y-1)+(x+1)];
			temp = temp + u[img_width*(y+1)+x];
			temp = temp + u[img_width*(y-1)+x];
			dmy_u[img_width*y+x]=temp/9.0;

			temp = v[img_width*y+x];
			temp = temp + v[img_width*y+(x-1)];
			temp = temp + v[img_width*y+(x+1)];
			temp = temp + v[img_width*(y+1)+(x-1)];
			temp = temp + v[img_width*(y+1)+(x+1)];
			temp = temp + v[img_width*(y-1)+(x-1)];
			temp = temp + v[img_width*(y-1)+(x+1)];
			temp = temp + v[img_width*(y+1)+x];
			temp = temp + v[img_width*(y-1)+x];
			dmy_v[img_width*y+x]=temp/9.0;

			temp = w[img_width*y+x];
			temp = temp + w[img_width*y+(x-1)];
			temp = temp + w[img_width*y+(x+1)];
			temp = temp + w[img_width*(y+1)+(x-1)];
			temp = temp + w[img_width*(y+1)+(x+1)];
			temp = temp + w[img_width*(y-1)+(x-1)];
			temp = temp + w[img_width*(y-1)+(x+1)];
			temp = temp + w[img_width*(y+1)+x];
			temp = temp + w[img_width*(y-1)+x];
			dmy_w[img_width*y+x]=temp/9.0;
		}

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{
			mr[img_width*y+x] = dmy_mr[img_width*y+x];
			u[img_width*y+x] = dmy_u[img_width*y+x];
			v[img_width*y+x] = dmy_v[img_width*y+x];
			w[img_width*y+x] = dmy_w[img_width*y+x];
		}


	delete [] dmy_mr;
	delete [] dmy_u;
	delete [] dmy_v;
	delete [] dmy_w;
*/
}

void CDataMRI::SaveDataSur(int idx, int fn_slice)
{
	CString filename;
	filename.Format("C:\\Documents and Settings\\bsc01r\\My Documents\\PhD\\data\\slices\\slice%i\\",
		fn_slice);

	CString toSaveMR, toSaveU, toSaveV, toSaveW;
	const int dataLength = 256*256*2;

	short* dmy_mr = new short[256*256];
	short* dmy_u = new short[256*256];
	short* dmy_v = new short[256*256];
	short* dmy_w = new short[256*256];

	int x, y;
	for(x=0; x<256; x++)
		for(y=0; y<256; y++)
		{

			if(x<img_width && y<img_height)
			{
				dmy_mr[256*y+x]=abs(u[img_width*y+x]);
				dmy_u[256*y+x]=u[img_width*y+x];
				dmy_v[256*y+x]=v[img_width*y+x];
				dmy_w[256*y+x]=w[img_width*y+x];
			}
			else
			{
				dmy_mr[256*y+x]=0;
				dmy_u[256*y+x]=0;
				dmy_v[256*y+x]=0;
				dmy_w[256*y+x]=0;
			}
		}

	if(idx<10)
	{
		toSaveMR.Format("%smr00%i.sur", filename, idx);
		toSaveU.Format("%su00%i.sur", filename, idx);
		toSaveV.Format("%sv00%i.sur", filename, idx);
		toSaveW.Format("%sw00%i.sur", filename, idx);
	}
	else
	{
		toSaveMR.Format("%smr0%i.sur", filename, idx);
		toSaveU.Format("%su0%i.sur", filename, idx);
		toSaveV.Format("%sv0%i.sur", filename, idx);
		toSaveW.Format("%sw0%i.sur", filename, idx);
	}

	// save MR file
	CFile fMR(toSaveMR, CFile::modeWrite|CFile::modeCreate);
	short a512[512];
	fMR.Write(a512,512);
	fMR.Write(dmy_mr,dataLength);
	fMR.Close();

	// save U file
	CFile fU(toSaveU, CFile::modeWrite|CFile::modeCreate);
	fU.Write(a512,512);
	fU.Write(dmy_u,dataLength);
	fU.Close();

	// save V file
	CFile fV(toSaveV, CFile::modeWrite|CFile::modeCreate);
	fV.Write(a512,512);
	fV.Write(dmy_v,dataLength);
	fV.Close();

	// save W file
	CFile fW(toSaveW, CFile::modeWrite|CFile::modeCreate);
	fW.Write(a512,512);
	fW.Write(dmy_w,dataLength);
	fW.Close();


}
