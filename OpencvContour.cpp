// OpencvContour.cpp: implementation of the COpencvContour class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "OpencvContour.h"
//#include <cv.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COpencvContour::COpencvContour()
{

}

COpencvContour::~COpencvContour()
{

}

BOOL COpencvContour::FitEllipse(CArray<CPoint, CPoint> & points, 
								CArray<CPoint, CPoint> & ellipse,
								int width, int height)
{
	// build CVPoint array from points array
	int i, nPoints;
	nPoints = points.GetSize();
	CvPoint2D32f* pointArray = new CvPoint2D32f[nPoints];
	for(i=0; i<nPoints; i++)
	{
		pointArray[i].x = (float)points.GetAt(i).x;
		pointArray[i].y = (float)points.GetAt(i).y;
	}

	// fit ellipse
	CvBox2D32f box;
	cvFitEllipse( pointArray, nPoints, &box);

	// create ipl target image
	IplImage *img8u = NULL;
	img8u = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		width, height, NULL, NULL, NULL, NULL );
	
	if( !img8u ) AfxMessageBox("Error creating ellipse target image.");
	
	iplAllocateImage( img8u, 1,0 );
	if( !img8u->imageData ) AfxMessageBox("Error allocating ellipse target image.");

	// draw ellipse onto target
	CvPoint centerPoint;
	centerPoint.x = (int)box.center.x;
	centerPoint.y = (int)box.center.y;
	CvSize axesSize;
	axesSize.width = (int)(box.size.width/2.0);
	axesSize.height = (int)(box.size.height/2.0);
	cvEllipse( img8u, centerPoint, axesSize, box.angle*(-1), 0, 360, 150, 1 );

	char sz266[100];
	sprintf(sz266,"Ellipse centre at (%i, %i) axes: (%i, %i) angle: %f", centerPoint.x, centerPoint.y,
		axesSize.width, axesSize.height, box.angle);
	AfxMessageBox(sz266);

	// extract ellipse points from target
	int x, y;
	unsigned char buffer[4];
	ellipse.RemoveAll();
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			iplGetPixel(img8u, x, y, buffer);
			if(buffer[0]!=0)
				ellipse.Add(CPoint(x,y));
		}

	iplDeallocate( img8u, IPL_IMAGE_ALL );

	return TRUE;
}

BOOL COpencvContour::FitCircle(CArray<CPoint, CPoint> & points, 
								CArray<CPoint, CPoint> & circle,
								int width, int height, BOOL silent,
								int* Cr, int* Cx, int* Cy)
{
	// fit circle
	int cRadius, cX, cY;
	if(fit_circle( points, cRadius, cX, cY) != TRUE)
	{
		AfxMessageBox("Error in circle fitting routine.");
		return FALSE;
	}

	char sz266[100];
	sprintf(sz266,"Circle centre at (%i, %i) radius: %i", cX, cY, cRadius);
	if(!silent)AfxMessageBox(sz266);

	GetCirclePoints(cRadius, cX, cY, circle);

	if(Cr) *Cr = cRadius;
	if(Cx) *Cx = cX;
	if(Cy) *Cy = cY;

	return TRUE;
}

BOOL COpencvContour::fit_circle(CArray<CPoint, CPoint> & points, int &radius, int &cx, int &cy)
{
	
	// implements circle fitting as devised by:

	// THOMAS SM, CHAN YT
	// A SIMPLE APPROACH FOR THE ESTIMATION OF CIRCULAR ARC CENTER AND ITS RADIUS
	// COMPUTER VISION GRAPHICS AND IMAGE PROCESSING 
	// 45 (3): 362-370 MAR 1989

	int N = points.GetSize();
	if(N<=2) return FALSE;

	// variables
	double a1, b1, b2, c1, c2;
	a1 = b1 = b2 = c1 = c2 = 0;

	// coordinate sums
	double Ex, Ey, Exx, Exy, Eyy, Exxx, Eyyy, Exxy, Exyy;
	Ex = Ey = Exx = Exy = Eyy = Exxx = Eyyy = Exxy = Exyy = 0;

	// build sums from point array
	int i;
	for(i=0; i<N; i++)
	{
		Ex = Ex + (double)points.GetAt(i).x;
		Exx = Exx + pow((double)points.GetAt(i).x,2.0);
		Ey = Ey + (double)points.GetAt(i).y;
		Eyy = Eyy + pow((double)points.GetAt(i).y,2.0);
		Exy = Exy + (double)points.GetAt(i).x * (double)points.GetAt(i).y;
		Exxx = Exxx + pow((double)points.GetAt(i).x,3.0);
		Eyyy = Eyyy + pow((double)points.GetAt(i).y,3.0);
		Exxy = Exxy + pow((double)points.GetAt(i).x,2.0) * (double)points.GetAt(i).y;
		Exyy = Exyy + pow((double)points.GetAt(i).y,2.0) * (double)points.GetAt(i).x;

	}

	// set variable values
	a1 = 2*(pow(Ex, 2.0)-N*Exx);
	b1 = 2*(Ex*Ey-N*Exy);
	b2 = 2*(pow(Ey, 2.0)-N*Eyy);
	c1 = Exx*Ex-N*Exxx+Ex*Eyy-N*Exyy;
	c2 = Exx*Ey-N*Eyyy+Ey*Eyy-N*Exxy;

	// set circle parameters
	double x = (c1*b2-c2*b1)/(a1*b2-b1*b1);
	double y = (a1*c2-b1*c1)/(a1*b2-b1*b1);
	double R = (1.0/N)*(Exx-2*Ex*x+N*pow(x,2.0)+Eyy-2*Ey*y+N*pow(y,2.0));

	double dmy;
	cx = (int)(modf(x,&dmy)>0.5?ceil(x):floor(x));
	cy = (int)(modf(y,&dmy)>0.5?ceil(y):floor(y));
	radius = (int)(modf(sqrt(R),&dmy)>0.5?ceil(sqrt(R)):floor(sqrt(R)));


	return TRUE;

}

BOOL COpencvContour::GetCirclePoints(int cRadius, int cX, int cY, 
									 CArray<CPoint, CPoint> & circle)
{
	// create ipl target image
	IplImage *img8u = NULL;
	img8u = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		cX+cRadius, cY+cRadius, NULL, NULL, NULL, NULL );
	
	if( !img8u ) 
	{
		AfxMessageBox("Error creating circle target image.");
		return FALSE;
	}
	
	iplAllocateImage( img8u, 1,0 );
	if( !img8u->imageData ) 
	{
		AfxMessageBox("Error allocating circle target image.");
		return FALSE;
	}

	// draw circle onto target
	CvPoint centerPoint;
	centerPoint.x = cX;
	centerPoint.y = cY;
	cvCircle( img8u, centerPoint, cRadius, 150, 1 );

	// extract circle points from target
	int x, y;
	unsigned char buffer[4];
	circle.RemoveAll();
	for(x=0; x<cX+cRadius; x++)
		for(y=0; y<cY+cRadius; y++)
		{
			iplGetPixel(img8u, x, y, buffer);
			if(buffer[0]!=0)
				circle.Add(CPoint(x,y));
		}

	iplDeallocate( img8u, IPL_IMAGE_ALL );

	return TRUE;

}
