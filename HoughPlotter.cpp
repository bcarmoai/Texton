// HoughPlotter.cpp: implementation of the CHoughPlotter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "HoughPlotter.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHoughPlotter::CHoughPlotter()
{

}

CHoughPlotter::~CHoughPlotter()
{

}

BOOL CHoughPlotter::PlotHough(int * img_data, int * img_hough, int img_width, int img_height, float min_diam, 
							  float max_diam, int circles)
{
	const int IMG_BACK = 0;
	const float RAD_INCR = 1.0;

	if(circles==1)
	{
		// Hough transform for circles
		int x, y;
		int* acc;
		acc = new int[img_width*img_height];
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				acc[img_width*y+x] = 0;

		int ang;
		float t;
		float pi = 3.141592654f;
		float x0, y0;
		int ix0, iy0;
		float r;
		double dmy;
		
		//r = min_diam / 2.0;
		for(r=min_diam/2.0f; r<=max_diam/2.0f; r=r+RAD_INCR)
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					if(img_data[img_width*y+x] != IMG_BACK)
						for(ang=0; ang<360; ang++)
						{	
							t = (ang*pi)/180;
							x0 = x-r*cos(t);
							y0 = y-r*sin(t);
							ix0 = (modf(x0,&dmy)>0.5?ceil(x0):floor(x0));
							iy0 = (modf(y0,&dmy)>0.5?ceil(y0):floor(y0));
							if(ix0<img_width && ix0 > 0 && iy0<img_height && iy0>0)
								acc[img_width*iy0+ix0]++;
						}

		// normalise accumulator and copy to target image
		int max_plot = 0;
		const float max_grey = 255.0;

		for(x=0; x<img_width;x++)
			for(y=0; y<img_height; y++)
				if(acc[img_width*y+x] > max_plot)
					max_plot = acc[img_width*y+x];
		
		float norm_ratio = max_grey/max_plot;

		for(x = 0; x< img_width; x++)
			for(y = 0; y<img_height; y++)
				img_hough[img_width*y+x] = (int)(acc[img_width*y+x]*norm_ratio);


	}
	else
	{
		// Hough transform for ellipses
		int x, y;
		int* acc;
		acc = new int[img_width*img_height];
		for(x=0; x<img_width; x++)
			for(y=0; y<img_height; y++)
				acc[img_width*y+x] = 0;

		int ang;
		float t;
		float pi = 3.141592654f;
		float x0, y0;
		int ix0, iy0;
		float a, b;
		double dmy;
		
		a = min_diam;
		b = max_diam;

		// for(r=min_diam/2.0f; r<=max_diam/2.0f; r=r+RAD_INCR)
			for(x=0; x<img_width; x++)
				for(y=0; y<img_height; y++)
					if(img_data[img_width*y+x] != IMG_BACK)
						for(ang=0; ang<360; ang++)
						{	
							t = (ang*pi)/180;
							x0 = x-a*cos(t);
							y0 = y-b*sin(t);
							ix0 = (modf(x0,&dmy)>0.5?ceil(x0):floor(x0));
							iy0 = (modf(y0,&dmy)>0.5?ceil(y0):floor(y0));
							if(ix0<img_width && ix0 > 0 && iy0<img_height && iy0>0)
								acc[img_width*iy0+ix0]++;
						}

		// normalise accumulator and copy to target image
		int max_plot = 0;
		const float max_grey = 255.0;

		for(x=0; x<img_width;x++)
			for(y=0; y<img_height; y++)
				if(acc[img_width*y+x] > max_plot)
					max_plot = acc[img_width*y+x];
		
		float norm_ratio = max_grey/max_plot;

		for(x = 0; x< img_width; x++)
			for(y = 0; y<img_height; y++)
				img_hough[img_width*y+x] = (int)(acc[img_width*y+x]*norm_ratio);

	}
	
	return TRUE;
}
