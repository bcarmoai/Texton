// Data3DMRI.cpp: implementation of the CData3DMRI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "Data3DMRI.h"
#include "DataMRI.h"
#include "HeartCFDVertex.h"
#include "HeartCFDCell.h"
#include <fstream.h>
#include <math.h>
#include <stdlib.h>

#include "VtkUtilities.h"
#include <vtkFloatArray.h>
#include <vtkStructuredGridWriter.h>
#include <vtkPointData.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CData3DMRI::CData3DMRI()
{
	mr = u = v = w = NULL;
	img_width = img_height = img_slices = 0;
}

CData3DMRI::~CData3DMRI()
{
	if(mr != NULL)
		delete [] mr;

	if(u != NULL)
		delete [] u;
	
	if(v != NULL)
		delete [] v;
	
	if(w != NULL)
		delete [] w;

}

void CData3DMRI::Reset()
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
	img_width = img_height = img_slices = 0;

}

void CData3DMRI::SetData(int width, int height, int slices, short * amri, short * au, short * av, short * aw)
{
	if(width < 0 || height < 0 || slices < 0 || amri==NULL || au==NULL || aw==NULL || av==NULL)
	{
		AfxMessageBox("Unexpected error: SetData() called with invalid arguments.");
		return;
	}

	Reset();

	img_width = width;
	img_height = height;
	img_slices = slices;

	mr = new short[width*height*slices];
	u = new short[width*height*slices];
	v = new short[width*height*slices];
	w = new short[width*height*slices];

	int x, y, z;
	for(z=0; z<img_slices; z++)
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
			{
				mr[z*img_width*img_height+y*img_width+x]
					= amri[z*img_width*img_height+y*img_width+x];
				u[z*img_width*img_height+y*img_width+x]
					= au[z*img_width*img_height+y*img_width+x];
				v[z*img_width*img_height+y*img_width+x]
					= av[z*img_width*img_height+y*img_width+x];
				w[z*img_width*img_height+y*img_width+x]
					= aw[z*img_width*img_height+y*img_width+x];
				
			}

}

CDataMRI* CData3DMRI::Get2dSlice(int slice)
{
	if(slice>=img_slices)
		return NULL;

	CDataMRI* slice2D = new CDataMRI;
	slice2D->img_width = img_width;
	slice2D->img_height = img_height;
	slice2D->mr = new short[img_width*img_height];
	slice2D->u = new short[img_width*img_height];
	slice2D->v = new short[img_width*img_height];
	slice2D->w = new short[img_width*img_height];

	
	int x, y, z;
	z = slice;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			{
				slice2D->mr[img_width*y+x] =
					mr[z*img_width*img_height+y*img_width+x];

				slice2D->u[img_width*y+x] =
					u[z*img_width*img_height+y*img_width+x];

				slice2D->v[img_width*y+x] =
					v[z*img_width*img_height+y*img_width+x];

				slice2D->w[img_width*y+x] =
					w[z*img_width*img_height+y*img_width+x];				
			}

	return slice2D;


}

int CData3DMRI::FloatToInt(float fnum)
{
	double dmy;

	if(fnum>0)
		if(modf(fnum,&dmy)==0.5)
			return ((int)fnum)%2 ? (int)ceil(fnum) : (int)floor(fnum);
		else	
			return (int)(modf(fnum,&dmy)>0.5? (int)ceil(fnum): (int)floor(fnum));
	else
		if(modf(fnum,&dmy)==0.5)
			return ((int)fnum)%2 ? (int)floor(fnum) : (int)ceil(fnum);
		else	
			return (int)(modf(fnum,&dmy)>0.5? (int)floor(fnum): (int)ceil(fnum));
}

void CData3DMRI::LoadDataCFD(CString filename)
{

	// allocate memory
	if(mr != NULL)
		delete [] mr;
	if(w != NULL)
		delete [] w;
	if(v != NULL)
		delete [] v;
	if(u != NULL)
		delete [] u;
	img_width = 46;
	img_height = 40;
	img_slices = 29;

	mr = new short[img_width*img_height*img_slices];
	u = new short[img_width*img_height*img_slices];
	v = new short[img_width*img_height*img_slices];
	w = new short[img_width*img_height*img_slices];

	// read data from file
	ifstream inf;
	inf.open(filename);
	
	const float x_mult = 0.511111f;
	const float y_mult = 0.512828f;
	const float z_mult = 0.517857f;

	float fx, fy, fz, fu, fv, fw, fp;
	int ix, iy, iz, index;
	int i;
	for(i=0; i<53360 /* length of file */; i++)
	{
		inf >> fx >> fy >> fz;
		ix = FloatToInt(fx/x_mult);
		iy = FloatToInt((fy+5.0f)/y_mult);
		iz = FloatToInt((fz+12.5f)/z_mult);

		if(ix >= img_width || iy >= img_height || iz >= img_slices
		|| ix < 0 || iy < 0 || iz < 0 )
		{
			CString msg;
			msg.Format("Unexpected coordinates: (x, y, z) %i %i %i", ix, iy, iz);
			AfxMessageBox(msg);
			continue;
		}

		index = img_width*img_height*iz+img_width*iy+ix;
		
		inf >> fu >> fv >> fw >> fp;

		u[index] = FloatToInt(fu*1000.0f);
		v[index] = FloatToInt(fv*1000.0f); 
		w[index] = FloatToInt(fw*1000.0f);

	}

	if(i<53360)
	{
		CString msg;
		msg.Format("Warning: data file smaller that expected.\nfile: %i bytes, expected: %i", i, 53360);
		AfxMessageBox(msg);
	}

	inf.close();

}

CDataMRI* CData3DMRI::Get2dXSlice(int x_coord)
{
	CDataMRI* slice2D = new CDataMRI;
	slice2D->img_width = img_slices;
	slice2D->img_height = img_height;
	slice2D->mr = new short[img_slices*img_height];
	slice2D->u = new short[img_slices*img_height];
	slice2D->v = new short[img_slices*img_height];
	slice2D->w = new short[img_slices*img_height];
	
	int x, y, z;
	x = x_coord;
	for(z=0; z<img_slices; z++)
		for(y=0; y<img_height; y++)
			{
				slice2D->mr[img_slices*y+z] =
					mr[z*img_width*img_height+y*img_width+x];

				slice2D->u[img_slices*y+z] =
					u[z*img_width*img_height+y*img_width+x];

				slice2D->v[img_slices*y+z] =
					v[z*img_width*img_height+y*img_width+x];

				slice2D->w[img_slices*y+z] =
					w[z*img_width*img_height+y*img_width+x];				

			}

	return slice2D;

}

CDataMRI* CData3DMRI::Get2dYSlice(int y_coord)
{
	CDataMRI* slice2D = new CDataMRI;
	slice2D->img_width = img_width;
	slice2D->img_height = img_slices;
	slice2D->mr = new short[img_width*img_slices];
	slice2D->u = new short[img_width*img_slices];
	slice2D->v = new short[img_width*img_slices];
	slice2D->w = new short[img_width*img_slices];

	int x, y, z;
	y = y_coord;
	for(z=0; z<img_slices; z++)
		for(x=0; x<img_width; x++)
			{
				slice2D->mr[img_width*z+x] =
					mr[z*img_width*img_height+y*img_width+x];

				slice2D->u[img_width*z+x] =
					u[z*img_width*img_height+y*img_width+x];

				slice2D->v[img_width*z+x] =
					v[z*img_width*img_height+y*img_width+x];

				slice2D->w[img_width*z+x] =
					w[z*img_width*img_height+y*img_width+x];				
			}

	return slice2D;

}

void CData3DMRI::LoadDataFrame(CString filename)
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
	img_width = 256;
	img_height = 256;
	img_slices = 1;

	mr = new short[img_width*img_height*img_slices];
	u = new short[img_width*img_height*img_slices];
	v = new short[img_width*img_height*img_slices];
	w = new short[img_width*img_height*img_slices];

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

	CFile fW(wFilename, CFile::modeRead);
	fW.Seek(512, CFile::begin );
	if(fW.Read(w,dataLength) != dataLength)
		AfxMessageBox("Warning: W data shorter than 256*256*2 bytes.");
	fW.Close();

}

void CData3DMRI::Zoom(CPoint upperLeft, CPoint bottomRight, int startSlice, int endSlice, float scaling)
{
	// note when calling, all coords zero-based

	// create destination arrays
	int destWidth, destHeight, destSlices;
	BOOL twoD = FALSE;

	destWidth = FloatToInt(scaling * ((float)(bottomRight.x-upperLeft.x + 1)));
	destHeight = FloatToInt(scaling * ((float)(bottomRight.y-upperLeft.y + 1)));

	if(endSlice-startSlice >= 1)
		destSlices = FloatToInt(scaling * ((float)(endSlice-startSlice + 1)));
	else
	{
		destSlices = 1;
		twoD = TRUE;
	}

	if(destWidth<=0 || destHeight<=0 || destSlices<=0/* || scaling == 1.0*/)
		return;

	if(destWidth*destHeight*destSlices > 900000)
	{
		CString sms;
		sms.Format("The ROI contains %i voxels. Processing may be very long. Continue zooming?", 
			destWidth*destHeight*destSlices);
		if(AfxMessageBox(sms, MB_YESNO)==IDNO)
			return;
	}

	short* mr_dest = new short[destWidth*destHeight*destSlices];
	short* u_dest = new short[destWidth*destHeight*destSlices];
	short* v_dest = new short[destWidth*destHeight*destSlices];
	short* w_dest = new short[destWidth*destHeight*destSlices];

	// go through destination dataset and set interpolated vortex values
	float coord_scale = scaling/*((float)(destWidth-1))/((float)(bottomRight.x-upperLeft.x))*/;
	int sx, sy, sz, dx, dy, dz;
	for(dx=0; dx < destWidth; dx++)
		for(dy=0; dy < destHeight; dy++)
			for(dz=0; dz < destSlices; dz++)
			{
				InterpolateVoxel(mr, mr_dest, dx, dy, dz, upperLeft.x, upperLeft.y, startSlice, coord_scale,
					destWidth, destHeight, twoD);
				InterpolateVoxel(u, u_dest, dx, dy, dz, upperLeft.x, upperLeft.y, startSlice, coord_scale,
					destWidth, destHeight, twoD);
				InterpolateVoxel(v, v_dest, dx, dy, dz, upperLeft.x, upperLeft.y, startSlice, coord_scale,
					destWidth, destHeight, twoD);
				InterpolateVoxel(w, w_dest, dx, dy, dz, upperLeft.x, upperLeft.y, startSlice, coord_scale,
					destWidth, destHeight, twoD);
			}


	// go through source dataset and set destination vertex voxel values
	// note all coords zero-based
	int error_code = 0;
	for(sx=upperLeft.x; sx <= bottomRight.x; sx++)
		for(sy=upperLeft.y; sy <= bottomRight.y; sy++)
			for(sz=startSlice; sz </*removed '=' 240303 */ endSlice; sz++)
			{
				dx = FloatToInt(coord_scale*((float)(sx-upperLeft.x)));
				dy = FloatToInt(coord_scale*((float)(sy-upperLeft.y)));
				dz = twoD ? 0 : FloatToInt(coord_scale*((float)(sz-startSlice)));

				if(dx >= destWidth)
					error_code = 1;
				if(dy >= destHeight)
					error_code = 2;
				if(dz >= destSlices)
					error_code = 3;

				if(dx >= destWidth || dy >= destHeight || dz >= destSlices)
				{
					CString sms133;
					sms133.Format("error %i", error_code);
					AfxMessageBox(sms133);
					continue;
				}

				mr_dest[dz*destWidth*destHeight+dy*destWidth+dx] =
					mr[sz*img_width*img_height+sy*img_width+sx];

				u_dest[dz*destWidth*destHeight+dy*destWidth+dx] =
					u[sz*img_width*img_height+sy*img_width+sx];
				
				v_dest[dz*destWidth*destHeight+dy*destWidth+dx] =
					v[sz*img_width*img_height+sy*img_width+sx];
				
				w_dest[dz*destWidth*destHeight+dy*destWidth+dx] =
					w[sz*img_width*img_height+sy*img_width+sx];
			}

	// delete original data and copy new one
	delete [] mr;
	delete [] u;
	delete [] v;
	delete [] w;
	img_width = destWidth;
	img_height = destHeight;
	img_slices = destSlices;
	mr = mr_dest;
	u = u_dest;
	v = v_dest;
	w = w_dest;

}

void CData3DMRI::InterpolateVoxel(short *src, short *dst, int dx, int dy, int dz, 
								  int sROIx, int sROIy, int sROIz, float scale,
								  int destWidth, int destHeight, BOOL twoD)
{

	/*static*/ int sxV000, syV000, szV000, sxV100, syV100, szV100,
		sxV010, syV010, szV010, sxV001, syV001, szV001,
		sxV101, syV101, szV101, sxV011, syV011, szV011,
		sxV110, syV110, szV110, sxV111, syV111, szV111;

	/*static*/ float sx, sy, sz, Vxyz;
	
	sxV000 = (int)floor(dx/scale);
	syV000 = (int)floor(dy/scale);
	szV000 = (int)floor(dz/scale);

	if(sxV000 >= img_width-1)
		sxV000 = img_width-2;

	if(syV000 >= img_height-1)
		syV000 = img_height-2;
	
	if(szV000 >= img_slices-1 && img_slices > 1)
		szV000 = img_slices-2;

	sxV100 = sxV000+1;
	syV100 = syV000;
	szV100 = szV000;

	sxV010 = sxV000;
	syV010 = syV000+1;
	szV010 = szV000;

	sxV001 = sxV000;
	syV001 = syV000;
	szV001 = szV000+1;

	sxV101 = sxV100;
	syV101 = syV000;
	szV101 = szV001;

	sxV011 = sxV000;
	syV011 = syV010;
	szV011 = szV001;

	sxV110 = sxV100;
	syV110 = syV010;
	szV110 = szV000;

	sxV111 = sxV100;
	syV111 = syV010;
	szV111 = szV001;

	sx = (float)dx/scale;
	sy = (float)dy/scale;
	sz = (float)dz/scale;

	if(!twoD)
	{
		Vxyz = 
			/* V000 (1-x) (1-y) (1-z) */
			(float)src[(szV000+sROIz)*img_width*img_height+(syV000+sROIy)*img_width+(sxV000+sROIx)]
			* ((float)sxV100-sx) * ((float)syV010-sy) * ((float)szV001-sz) +
			
			/* V100 x (1-y) (1-z) */
			(float)src[(szV100+sROIz)*img_width*img_height+(syV100+sROIy)*img_width+(sxV100+sROIx)]
			* (sx-(float)sxV000) * ((float)syV010-sy) * ((float)szV001-sz) +
			
			/* V010 (1-x) y (1-z) */
			(float)src[(szV010+sROIz)*img_width*img_height+(syV010+sROIy)*img_width+(sxV010+sROIx)]
			* ((float)sxV100-sx) * (sy-(float)syV000) * ((float)szV001-sz) +
			
			/* V001 (1-x) (1-y) z */
			(float)src[(szV001+sROIz)*img_width*img_height+(syV001+sROIy)*img_width+(sxV001+sROIx)]
			* ((float)sxV100-sx) * ((float)syV010-sy) * (sz-(float)szV000) +
			
			/* V101 x (1-y) z */
			(float)src[(szV101+sROIz)*img_width*img_height+(syV101+sROIy)*img_width+(sxV101+sROIx)]
			* (sx-(float)sxV000) * ((float)syV010-sy) * (sz-(float)szV000) +
			
			/* V011 (1-x) y z */
			(float)src[(szV011+sROIz)*img_width*img_height+(syV011+sROIy)*img_width+(sxV011+sROIx)]
			* ((float)sxV100-sx) * (sy-(float)syV000) * (sz-(float)szV000) +
			
			/* V110 x y (1-z) */
			(float)src[(szV110+sROIz)*img_width*img_height+(syV110+sROIy)*img_width+(sxV110+sROIx)]
			* (sx-(float)sxV000) * (sy-(float)syV000) * ((float)szV001-(sz)) +
			
			/* V111 x y z */
			(float)src[(szV111+sROIz)*img_width*img_height+(syV111+sROIy)*img_width+(sxV111+sROIx)]
			* (sx-(float)sxV000) * (sy-(float)syV000) * (sz-(float)szV000);
	}
	else
	{
		Vxyz = 
			/* V000 (1-x) (1-y) (1-z) */
			(float)src[(szV000+sROIz)*img_width*img_height+(syV000+sROIy)*img_width+(sxV000+sROIx)]
			* ((float)sxV100-sx) * ((float)syV010-sy) +
			
			/* V100 x (1-y) (1-z) */
			(float)src[(szV000+sROIz)*img_width*img_height+(syV100+sROIy)*img_width+(sxV100+sROIx)]
			* (sx-(float)sxV000) * ((float)syV010-sy) +
			
			/* V010 (1-x) y (1-z) */
			(float)src[(szV000+sROIz)*img_width*img_height+(syV010+sROIy)*img_width+(sxV010+sROIx)]
			* ((float)sxV100-sx) * (sy-(float)syV000) +
			
			/* V110 x y (1-z) */
			(float)src[(szV000+sROIz)*img_width*img_height+(syV110+sROIy)*img_width+(sxV110+sROIx)]
			* (sx-(float)sxV000) * (sy-(float)syV000);

	}

	dst[dz*destWidth*destHeight+dy*destWidth+dx] = (short)FloatToInt(Vxyz);
		
}

void CData3DMRI::FlipU()
{
	int x, y, z;
	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				u[z*img_width*img_height+y*img_width+x] = u[z*img_width*img_height+y*img_width+x] * -1;

}

void CData3DMRI::FlipV()
{
	int x, y, z;
	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				v[z*img_width*img_height+y*img_width+x] = v[z*img_width*img_height+y*img_width+x] * -1;

}

void CData3DMRI::FlipW()
{
	int x, y, z;
	for(x=0; x<img_width;x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
				w[z*img_width*img_height+y*img_width+x] = w[z*img_width*img_height+y*img_width+x] * -1;

}

void CData3DMRI::LoadDataHeartCFD(CString filename, int cfdHeight)
{
	DataHeartCFDInterpolation(filename, cfdHeight);
}

void CData3DMRI::SaveDataSur(int idx, int zb_slice)
{
	CDataMRI* pmrData;
	pmrData = Get2dXSlice(zb_slice);
	pmrData->SaveDataSur(idx, zb_slice);
	
}

void CData3DMRI::SavePointCount(int* point_count, int pcount)
{

	int x, y, z, zCount, tCount;
	zCount = tCount = 0;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				tCount++;
				if(point_count[z*img_width*img_height+y*img_width+x]==0)
					zCount++;
			}
	
	CString sms;
	sms.Format("point_count has %i non-zeros. Total positions is %i.", tCount-zCount, pcount-1);
	AfxMessageBox(sms);

}

void CData3DMRI::DataHeartCFDAveraging(CString filename, int cfdHeight)
{
	const double xAspect = 2.6116305899E-01; 
	const double zAspect = 7.7501522209E-01;
	// note y aspect is 1, see Data irregularity analysis.xls

	// allocate memory
	if(mr != NULL)
		delete [] mr;
	if(w != NULL)
		delete [] w;
	if(v != NULL)
		delete [] v;
	if(u != NULL)
		delete [] u;

	img_width  = ceil(cfdHeight*xAspect);
	img_height = cfdHeight;
	img_slices = ceil(cfdHeight*zAspect);

	mr = new short[img_width*img_height*img_slices];
	u = new short[img_width*img_height*img_slices];
	v = new short[img_width*img_height*img_slices];
	w = new short[img_width*img_height*img_slices];

	// temporary velocity holders
	float* uf = new float[img_width*img_height*img_slices];
	float* vf = new float[img_width*img_height*img_slices];
	float* wf = new float[img_width*img_height*img_slices];

	// data point count at each velocity voxel
	int* point_count = new int[img_width*img_height*img_slices];

	// zero all
	for(int i=0; i<img_width*img_height*img_slices; i++)
		point_count[i] = uf[i] = vf[i] = wf[i] = 0.0f;

	// read data from file
	// positions file
	ifstream infgeo;
	infgeo.open(filename);

	CString vel_filename;
	vel_filename = filename;
	int dotpos = vel_filename.ReverseFind('.');
	vel_filename = vel_filename.Left(dotpos)+CString("velocity");

	// velocities file
	ifstream infvelocity;
	infvelocity.open(vel_filename);

	// look for '43488' marker in geo file, before first coordinate
	char sdmy[20];
	while(!infgeo.eof())
	{
		infgeo >> sdmy;

		if(strcmp(sdmy, "43488")==0)
			break;
	}

	// look for 'Vector' marker in velocity file, before first velocity
	while(!infvelocity.eof())
	{
		infvelocity >> sdmy;

		if(strcmp(sdmy, "Vector")==0)
			break;
	}

	float xPos, yPos, zPos;
	int xiPos, yiPos, ziPos;
	float xVel, yVel, zVel;

	// transform constants for y=mx+b
	// see Data irregularity analysis.xls for numbers
	float trf_mx, trf_my, trf_mz, trf_bx, trf_by, trf_bz;
	trf_mx = (float)(img_width-1)/(6.2569800540E-02);
	trf_my = (float)(img_height-1)/(2.3958135880E-01);
	trf_mz = (float)(img_slices-1)/(1.8567920000E-01);
	trf_bx = (-1.0f)*trf_mx*-6.2577300000E-02;
	trf_by = (-1.0f)*trf_my*2.7641200000E-05;
	trf_bz = (-1.0f)*trf_mz*2.1516800000E-02;

	float ftest1 = 2.76e-5;
	float ftest2 = 2.40e-1;

	yiPos = FloatToInt(trf_my*ftest1 + trf_by);
	yiPos = FloatToInt(trf_my*ftest2 + trf_by);

	// vtk data
	vtkFloatArray *fpoints = vtkFloatArray::New();
	vtkPoints* points = vtkPoints::New();
	vtkFloatArray *fspoints = vtkFloatArray::New();
	fspoints->SetNumberOfComponents(1);
	fpoints->SetNumberOfComponents(3);
	const BOOL VTK_SAVE = TRUE;

	int pcount=0;
	while(!infgeo.eof()&&!infvelocity.eof())
	{
		pcount++;
		if(pcount>43488)
			break;

		// get position from geo file
		infgeo >> xPos >> yPos >> zPos;

		// transform position to voxel coordinates
		xiPos = FloatToInt( trf_mx * xPos + trf_bx);
		yiPos = FloatToInt( trf_my * yPos + trf_by);
		ziPos = FloatToInt( trf_mz * zPos + trf_bz);
		
		if(xiPos>=img_width || xiPos < 0 || yiPos>=img_height || yiPos < 0 ||
		   ziPos>=img_slices || ziPos < 0)
		{
			CString sms;
			sms.Format("bad coordinate one of: %i %i %i", xiPos, yiPos, ziPos);
			AfxMessageBox(sms);
		}

		// set velocity from velocity file
		infvelocity >> xVel >> yVel >> zVel;

		//      185     191     192     186     119     125     126     120
		// save to vtk file
		if(VTK_SAVE)
			if(pcount==67||pcount==73||pcount==74||pcount==68||pcount==1||pcount==7||pcount==8||pcount==2)
			//if(pcount==68||pcount==74||pcount==75||pcount==69||pcount==2||pcount==8||pcount==9||pcount==3)
			//if(pcount==16735||pcount==16740||pcount==16741||pcount==16736||pcount==16680||pcount==16685||pcount==16686||pcount==16681)
			//if(pcount==185||pcount==191||pcount==192||pcount==186||pcount==119||pcount==125||pcount==126||pcount==120)
			{
				// fspoints->InsertNextTuple1(sqrt(pow(xVel,2.0)+pow(yVel,2.0)+pow(zVel,2.0)));
				points->InsertNextPoint(xPos, yPos, zPos);
				fpoints->InsertNextTuple3(xVel, yVel, zVel);
				//fspoints->InsertNextTuple1();
				/*fpoints->InsertNextTuple3(pcount==185?1:pcount==191?2:pcount==192?3:pcount==186?4:0.5, 
										  pcount==185?1:pcount==191?2:pcount==192?3:pcount==186?4:0.5,
										  pcount==185?1:pcount==191?2:pcount==192?3:pcount==186?4:0.5);*/
			}

		uf[ziPos*img_width*img_height+yiPos*img_width+xiPos] += xVel;
		vf[ziPos*img_width*img_height+yiPos*img_width+xiPos] += yVel;
		wf[ziPos*img_width*img_height+yiPos*img_width+xiPos] += zVel;

		// increment point count
		point_count[ziPos*img_width*img_height+yiPos*img_width+xiPos]++;

	}

	// uncomment to count empty voxels
	// note we're passing local scope pointer
	// SavePointCount(point_count, pcount);

	// copy float to short data and normalise by no. data points
	float short_norm_m = /* short limits */ (32767.0f+32768.0f)/(2.0*2.20338f); /* velocity range in all files */
	// float short_norm_b = 5813.0f; we don't use b because we want zero of rescaled to match original zero

	for(i=0; i<img_width*img_height*img_slices; i++)
	{
		mr[i]=0;
		u[i]=FloatToInt((uf[i]/(float)point_count[i])*short_norm_m*.1);
		v[i]=FloatToInt((vf[i]/(float)point_count[i])*short_norm_m*.1);
		w[i]=FloatToInt((wf[i]/(float)point_count[i])*short_norm_m*.1);
	}

	infgeo.close();
	infvelocity.close();

	delete [] uf;
	delete [] vf;
	delete [] wf;
	delete [] point_count;

	// vtk finishing
	if(VTK_SAVE)
	{
		vtkStructuredGrid *sgrid = vtkStructuredGrid::New();
		sgrid->SetDimensions(img_width, img_height, img_slices);
		sgrid->SetPoints(points);
		sgrid->GetPointData()->SetScalars(fspoints);
		sgrid->GetPointData()->SetVectors(fpoints);

		vtkStructuredGridWriter* swriter = vtkStructuredGridWriter::New();
		swriter->SetInput(sgrid);
		swriter->SetFileName("c:\\testHCFD.vtk");
		swriter->Write();

		sgrid->Delete();
		swriter->Delete();
	}

	fpoints->Delete();
	fspoints->Delete();
	points->Delete();


}

void CData3DMRI::DataHeartCFDInterpolation(CString filename, int cfdHeight)
{

	// open geo file with coordinates and connectivity
	ifstream infgeo;
	infgeo.open(filename);

	CString vel_filename;
	vel_filename = filename;
	int dotpos = vel_filename.ReverseFind('.');
	vel_filename = vel_filename.Left(dotpos)+CString(".velocity");

	// open velocities file
	ifstream infvelocity;
	infvelocity.open(vel_filename);

	// look for '43488' marker in geo file, before first coordinate
	char sdmy[20];
	while(!infgeo.eof())
	{
		infgeo >> sdmy;

		if(strcmp(sdmy, "43488")==0)
			break;
	}

	// look for 'Vector' marker in velocity file, before first velocity
	while(!infvelocity.eof())
	{
		infvelocity >> sdmy;

		if(strcmp(sdmy, "Vector")==0)
			break;
	}

	// load all positions into 1-based ordered vertex array
	CHeartCFDVertex* vertex_array = new CHeartCFDVertex[43488+1];

	int vertex_no = 0;

	while(!infgeo.eof()&&!infvelocity.eof())
	{
		vertex_no++;
		if(vertex_no>43488)
			break;

		// get position from geo file
		infgeo >> vertex_array[vertex_no].x >> vertex_array[vertex_no].y >> vertex_array[vertex_no].z;

		// set velocity from velocity file
		infvelocity >> vertex_array[vertex_no].xVel >> vertex_array[vertex_no].yVel >> vertex_array[vertex_no].zVel;

	}

	// check that we're at the right spot in the geo file
	char dmy[20];
	infgeo >> dmy;
	if(strcmp(dmy,"part")!=0)
	{
		CString sms;
		sms.Format("Warning: dummy variable should read \"part\" instead of %s.", dmy);
		AfxMessageBox(sms);
	}

	// look for first connectivity number
	while(!infgeo.eof())
	{
		infgeo >> dmy;
		if(strcmp(dmy, "41000")==0)
			break;
	}

	// read in connectivity data
	CHeartCFDCell* cell_array = new CHeartCFDCell[41000];
	int cell_count = 0;
	while(!infgeo.eof())
	{
		infgeo >> cell_array[cell_count].vertex_points[1] >> cell_array[cell_count].vertex_points[2] 
			   >> cell_array[cell_count].vertex_points[3] >> cell_array[cell_count].vertex_points[4] 
			   >> cell_array[cell_count].vertex_points[5] >> cell_array[cell_count].vertex_points[6] 
			   >> cell_array[cell_count].vertex_points[7] >> cell_array[cell_count].vertex_points[8];
		cell_count++;

		if(cell_count==41000)
			break;
	}

	if(cell_count!=41000)
	{
		CString sms;
		sms.Format("Warning: read %i instead of expected 41000 cells.", cell_count);
		AfxMessageBox(sms);
	}
	
	// go through all cells and create interpolated data

	const double xAspect = 2.6116305899E-01; 
	const double zAspect = 7.7501522209E-01;
	// note y aspect is 1, see Data irregularity analysis.xls

	// allocate memory
	if(mr != NULL)
		delete [] mr;
	if(w != NULL)
		delete [] w;
	if(v != NULL)
		delete [] v;
	if(u != NULL)
		delete [] u;

	img_width  = ceil(cfdHeight*xAspect);
	img_height = cfdHeight;
	img_slices = ceil(cfdHeight*zAspect);

	mr = new short[img_width*img_height*img_slices];
	u = new short[img_width*img_height*img_slices];
	v = new short[img_width*img_height*img_slices];
	w = new short[img_width*img_height*img_slices];

	// temporary velocity holders
	float* uf = new float[img_width*img_height*img_slices];
	float* vf = new float[img_width*img_height*img_slices];
	float* wf = new float[img_width*img_height*img_slices];

	// data point count at each velocity voxel
	int* point_count = new int[img_width*img_height*img_slices];

	// zero all
	for(int i=0; i<img_width*img_height*img_slices; i++)
		point_count[i] = uf[i] = vf[i] = wf[i] = 0.0f;

	// fill float voxels with interpolated data
	int cell_no;
	for(cell_no = 0; cell_no < 41000; cell_no++)
		cell_array[cell_no].CreateInterpolatedData(vertex_array, cfdHeight, uf, vf, wf, point_count);

	// fill short voxels with normalised data
	// copy float to short data and normalise by no. data points

	int short_range=(1<<16);
	float short_norm_m = /* short limits */ short_range/(2.0*2.20338f); /* velocity range in all files */

	for(i=0; i<img_width*img_height*img_slices; i++)
	{
		mr[i]=0;
		u[i]=FloatToInt((uf[i]/(float)point_count[i])*short_norm_m);
		v[i]=FloatToInt((vf[i]/(float)point_count[i])*short_norm_m);
		w[i]=FloatToInt((wf[i]/(float)point_count[i])*short_norm_m);
	}

	delete [] uf;
	delete [] vf;
	delete [] wf;
	delete [] point_count;
	delete [] vertex_array;
	delete [] cell_array;


	// cell_array[20000].SaveVtkCell();

	////////// ***** COMMENT BLOCK BELOW SEE LOG 8/5/03 ***** //////////
	/*
	// go through voxel dataset and fill it with interpolated cell data
	const double xAspect = 2.6116305899E-01; 
	const double zAspect = 7.7501522209E-01;
	// note y aspect is 1, see Data irregularity analysis.xls

	// allocate memory
	if(mr != NULL)
		delete [] mr;
	if(w != NULL)
		delete [] w;
	if(v != NULL)
		delete [] v;
	if(u != NULL)
		delete [] u;

	img_width  = ceil(cfdHeight*xAspect);
	img_height = cfdHeight;
	img_slices = ceil(cfdHeight*zAspect);

	mr = new short[img_width*img_height*img_slices];
	u = new short[img_width*img_height*img_slices];
	v = new short[img_width*img_height*img_slices];
	w = new short[img_width*img_height*img_slices];

	// zero all
	for(int i=0; i<img_width*img_height*img_slices; i++)
		mr[i] = u[i] = v[i] = w[i] = 0.0f;

	// copy float to short data and normalise by no. data points
	float short_norm_m = // short limits 
		(32767.0f+32768.0f)/(2.0*2.20338f); // velocity range in all files
	// float short_norm_b = 5813.0f; we don't use b because we want zero of rescaled to match original zero
	float dmyu, dmyv, dmyw;
	int x, y, z;
	float fx, fy, fz;
	float cur_dist, min_dist=99999;
	int min_cell=-1;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			for(z=0; z<img_slices; z++)
			{
				x = 12; z = 10 ; y = 85;
				DataHeartCFDConvertCoordinates(x,y,z,cfdHeight,fx,fy,fz);

				for(cell_no=0; cell_no<41000; cell_no++)
				{
					cur_dist = cell_array[cell_no].GetCellPoint(fx, fy, fx, dmyu, dmyv, dmyw);
					if(cur_dist>0.1)
						continue;
					else
					{
						if(min_dist>cur_dist)
						{
							min_dist = cur_dist;
							min_cell = cell_no;
						}

					}
				}

				if(min_cell !=-1)
				{
					cell_array[min_cell].GetCellPoint(fx, fy, fx, dmyu, dmyv, dmyw);
					u[i]=FloatToInt( dmyu * short_norm_m * .1 );
					v[i]=FloatToInt( dmyv * short_norm_m * .1 );
					w[i]=FloatToInt( dmyw * short_norm_m * .1 );
				}
				else
					continue;
			
			}

	*/


}

void CData3DMRI::DataHeartCFDConvertCoordinates(int x, int y, int z, 
					int cfdHeight, float& fx, float& fy, float& fz)
{
	const double xAspect = 2.6116305899E-01; 
	const double zAspect = 7.7501522209E-01;
	// yAspect is 1.0
	
	// transform constants for y=mx+b
	// see Data irregularity analysis.xls for numbers
	const float trf_mx = (float)(6.2569800540E-02)/(img_width-1);
	const float trf_my = (float)(2.3958135880E-01)/(img_height-1);
	const float trf_mz = (float)(1.8567920000E-01)/(img_slices-1);
	const float trf_bx = -6.2577300000E-02;
	const float trf_by = 2.7641200000E-05;
	const float trf_bz = 2.1516800000E-02;

	fx = trf_mx * x + trf_bx;
	fy = trf_my * y + trf_by;
	fz = trf_mz * z + trf_bz;

}

CHeartCFDCell* CData3DMRI::DataHeartCFDGetClosestCell(CHeartCFDCell* cell_array, 
													  CHeartCFDVertex* vertex_array,
													  float x, float y, float z)
{
	float min_dist, cur_dist;
	int min_cell;

	// get all cell vertex distances
	min_dist = cell_array[0].GetTotalPointDistance(vertex_array, x, y, z);
	min_cell = 0;
	for(int i=1; i<41000; i++)
	{
		cur_dist = cell_array[i].GetTotalPointDistance(vertex_array, x, y, z);;

		if(cur_dist<min_dist)
		{
			min_dist = cur_dist;
			min_cell = i;
		}
	}

	return &cell_array[min_cell];
}

void CData3DMRI::SaveDataHeartCFD(CString filename)
{
	ofstream of;
	of.open(filename);

	of << img_width << " " << img_height << " " << img_slices << "\n\r";

	const int totalsize = img_width*img_height*img_slices;

	for(int i = 0; i<totalsize; i++)
	{
		of << mr[i] << " " << u[i] << " " << v[i] << " " << w[i] << "\n\r";
	}
	
}

void CData3DMRI::LoadDataHeartCFDTexton(CString filename)
{
	ifstream inf;
	inf.open(filename);

	inf >> img_width >> img_height >> img_slices;

	const int totalsize = img_width*img_height*img_slices;

	// allocate memory
	if(mr != NULL)
		delete [] mr;
	if(w != NULL)
		delete [] w;
	if(v != NULL)
		delete [] v;
	if(u != NULL)
		delete [] u;
	mr = new short[img_width*img_height*img_slices];
	u = new short[img_width*img_height*img_slices];
	v = new short[img_width*img_height*img_slices];
	w = new short[img_width*img_height*img_slices];

	for(int i = 0; i<totalsize; i++)
	{
		inf >> mr[i] >> u[i] >> v[i] >> w[i];
/*		mr[i] = FloatToInt((float)mr[i]/16.0f);
		u[i] = FloatToInt((float)u[i]/16.0f);
		v[i] = FloatToInt((float)v[i]/16.0f);
		w[i] = FloatToInt((float)w[i]/16.0f);
*/
		mr[i] = FloatToInt((float)mr[i]);
		u[i] = FloatToInt((float)u[i]);
		v[i] = FloatToInt((float)v[i]);
		w[i] = FloatToInt((float)w[i]);

/*		if(mr[i]>1800 || mr[i] < -1800 ||
		   u[i]>1800  || u[i] < -1800  ||
		   v[i]>1800  || v[i] < -1800  ||
		   w[i]>1800  || w[i] < -1800 )
		   AfxMessageBox("oops");*/
	}



}

int get_int(unsigned char* data)
{
	int result = 0;

	for(int i = 0; i<4; i++)
		result |= data[i]<<(8*i);

	return result;
}

short get_short(unsigned char* data)
{
	int result = 0;

	for(int i = 0; i<2; i++)
		result |= data[i]<<(8*i);

	return result;
}

void CData3DMRI::LoadDataHeart7D(CString filename, int timeframe)
{

	int no_images, size_x, size_y;

	unsigned char* sz_buf = (unsigned char*)malloc(12);	

	FILE* pFile;

	pFile = fopen(filename, "rb");
	fread(sz_buf, 1, 12, pFile);

	no_images = get_int(sz_buf);
	size_x = get_int(sz_buf+4);
	size_y = get_int(sz_buf+8);


	// allocate memory
	if(mr != NULL)
		delete [] mr;
	if(w != NULL)
		delete [] w;
	if(v != NULL)
		delete [] v;
	if(u != NULL)
		delete [] u;

	int pixel_idx, frame_index, file_index, time_index;
	int component_offset = 1152;
	int dataset_offset = 0; /* 0 for A, 1 for B */
	int slice_offset = 96;

	no_images = slice_offset/2;
	mr = new short[no_images*size_x*size_y];
	u = new short[no_images*size_x*size_y];
	v = new short[no_images*size_x*size_y];
	w = new short[no_images*size_x*size_y];

	int framelen = 2*size_x*size_y;
	unsigned char *frame_data = (unsigned char*)malloc(framelen);

	time_index = timeframe;

	for(frame_index = dataset_offset; frame_index < slice_offset; frame_index=frame_index+2)
	{
		file_index = (slice_offset*time_index + frame_index)*framelen + 12;

		fseek(pFile, file_index, SEEK_SET);
		fread(frame_data, framelen, 1, pFile);
		for(pixel_idx=0; pixel_idx<size_x*size_y; pixel_idx++)
			u[(frame_index/2)*size_x*size_y+pixel_idx] = get_short(frame_data+pixel_idx*2) - 2045;

		file_index = (slice_offset*time_index + component_offset + frame_index)*framelen + 12;

		fseek(pFile, file_index, SEEK_SET);
		fread(frame_data, framelen, 1, pFile);
		for(pixel_idx=0; pixel_idx<size_x*size_y; pixel_idx++)
			v[(frame_index/2)*size_x*size_y+pixel_idx] = get_short(frame_data+pixel_idx*2) - 2045;

		file_index = (slice_offset*time_index + (2*component_offset) + frame_index)*framelen + 12;

		fseek(pFile, file_index, SEEK_SET);
		fread(frame_data, framelen, 1, pFile);
		for(pixel_idx=0; pixel_idx<size_x*size_y; pixel_idx++)
			w[(frame_index/2)*size_x*size_y+pixel_idx] = get_short(frame_data+pixel_idx*2) - 2045;

	}

/*	
	for(j=0; j<no_images; j++)
	{
		fread(frame_data, framelen, 1, pFile);
		for(i=0; i<size_x*size_y; i++)
			u[j*size_x*size_y+i] = get_short(frame_data+i*2);
	}

	fseek(pFile, 2*1152-2, SEEK_SET);
	for(j=0; j<no_images; j++)
	{
		fread(frame_data, framelen, 1, pFile);
		for(i=0; i<size_x*size_y; i++)
			v[j*size_x*size_y+i] = get_short(frame_data+i*2);
	}

	fseek(pFile, 4*1152-4, SEEK_SET);
	for(j=0; j<no_images; j++)
	{
		fread(frame_data, framelen, 1, pFile);
		for(i=0; i<size_x*size_y; i++)
			w[j*size_x*size_y+i] = get_short(frame_data+i*2);
	}
*/
	img_width = size_x;
	img_height = size_y;
	img_slices = no_images;

	if(FALSE)
	{
		ofstream of;
		of.open("c:\\slice_error.txt");
		int k, x, y;
		double error;
		for(k=1; k<img_slices-2; k=k+2)
		{
			error = 0.0;
			
			for(y=0; y<img_height; y++)
				for(x=0; x<img_width; x++)
					error = error + 
						(fabs(u[img_width*img_height*k+img_width*y+x] 
						        - u[img_width*img_height*(k+2)+img_width*y+x]));				

			of << k << " " << error << "\n" << flush;
		}

	}

	fclose(pFile);

	

}

void CData3DMRI::SaveDataMatlab(CString filename)
{
	ofstream of;
	of.open(filename);

	int x, y, z;

	// U array
	for(z=0; z<img_slices; z++)
	{
		of << "U(:,:," << z+1 << ")=[" ;

		for(y=0; y<img_height; y++)
		{
			for(x=0; x<img_width; x++)
			{
				of << u[img_width*img_height*z+img_width*y+x] << " ";

			}

			of << ";\n";
		}

		of << "];\n";
	}

	// V array
	for(z=0; z<img_slices; z++)
	{
		of << "V(:,:," << z+1 << ")=[" ;

		for(y=0; y<img_height; y++)
		{
			for(x=0; x<img_width; x++)
			{
				of << v[img_width*img_height*z+img_width*y+x] << " ";

			}

			of << ";\n";
		}

		of << "];\n";
	}

	// W array
	for(z=0; z<img_slices; z++)
	{
		of << "W(:,:," << z+1 << ")=[" ;

		for(y=0; y<img_height; y++)
		{
			for(x=0; x<img_width; x++)
			{
				of << w[img_width*img_height*z+img_width*y+x] << " ";

			}

			of << ";\n";
		}

		of << "];\n";
	}

	// position arrays
	of << "[X,Y,Z] = meshgrid(1:"<<img_width<<",1:"<<img_height<<",1:"<<img_slices<<");\n";

	// non-null positions
	of << "[C]" << "=[" ;
	for(z=0; z<img_slices; z++)
	{
		for(y=0; y<img_height; y++)
		{
			for(x=0; x<img_width; x++)
			{
				if(u[img_width*img_height*z+img_width*y+x] +
				   v[img_width*img_height*z+img_width*y+x] +
				   w[img_width*img_height*z+img_width*y+x] != 0)
					of << x << " " << y << " " << z << "\n";

			}

		}

	}

	of << "];\n";

	of << "CX = C(:,1,:);\nCY = C(:,2,:);\nCZ = C(:,3,:);\n";

}
