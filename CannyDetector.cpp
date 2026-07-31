// CannyDetector.cpp: implementation of the CCannyDetector class.
//
//////////////////////////////////////////////////////////////////////

// Functions CannyCore, Gaussian, Follow, Hypotenuse, Thresholding
// are from code PgmCanny.c, part of original header below:

/**
** 		      Robot Vision Group
** 		Dept. of Artificial Intelligence
** 		    University of Edinburgh
** 
** Authors: Bob Fisher, Dave Croft, A Fitzgibbon
** Date: September 86
** Program: canny.c
** Current Maintainer: andrewfg@ed.ac.uk
**
** Purpose: to apply a simple symmetric canny operator to any size image
**
**
**/

/*
**	integrated into Texton by Bernard Carmo in December 2001
*/


// BUG 21/12/01 : Program will fail for some image sizes, reason unknown

#include "stdafx.h"
#include "Texton.h"
#include "CannyDetector.h"

#include <math.h>
#include <malloc.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCannyDetector::CCannyDetector()
{
	ORIENT_SCALE = 40.0;
}

CCannyDetector::~CCannyDetector()
{

}

double CCannyDetector::Hypotenuse(double x, double y)
{
    if (x==0.0 && y==0.0) return(0.0);
    else return(hypot(x,y));
}

double CCannyDetector::Gaussian(double x, double s)
{
    return(exp((-x*x)/(2*s*s)));
}

int CCannyDetector::CannyCore(double s, int cols, int rows, 
							   unsigned char * data, 
							   unsigned char * derivative_mag, 
							   unsigned char * magnitude, 
							   unsigned char * orientation)
{
  int filter_width;               /* length of 1-D gaussian mask */
  float *gsmooth_x,*gsmooth_y;
  float *derivative_x,*derivative_y;
  int i,j,k,n;            /* counters */
  int t;                /* temp. grad magnitude variable */
  double a,b,c,d,g0;        /* mask generation intermediate vars*/
  double ux,uy;
  double t1,t2;
  double G[20],dG[20],D2G[20];    /*gaussian & derivative filter masks*/
  double gc,gn,gs,gw,ge,gnw,gne,gsw,gse;
  int picsize,jstart,jlimit;
  int ilimit;
  register jfactor;
  int kfactor1,kfactor2;
  int kfactor;
  register cindex,nindex,sindex,windex,eindex,nwindex,neindex,swindex,seindex;
  int low=1,high=255;        /* tracker hysteresis parameters */
  int cols_plus,cols_minus;    /*cols+1 and cols-1 respectively*/
  int mag_overflow_count=0;    /* used to measure how oft mag array overflows */
  int mag_underflow_count=0;    /* used to measure how oft mag array underflows */

  picsize=cols*rows;        /* picture area */

  /* calc coeffs for 1-dimensional G, dG/dn and
     Delta-squared G filters */
  for(n=0; n<20; ++n)
    {
      a=Gaussian(((double) n),s);
      if (a>0.005 || n<2)
        {
	  b=Gaussian((double)n-0.5,s);
	  c=Gaussian((double)n+0.5,s);
	  d=Gaussian((double)n,s*0.5);
	  // char sz123[100];
	  // sprintf(sz123,"a,b,c: %lf,%lf,%lf\n",a,b,c);
	  // AfxMessageBox(sz123);
	  G[n]=(a+b+c)/3/(6.283185*s*s);
	  dG[n]=c-b;
	  D2G[n]=1.6*d-a; /* DOG */
	  // sprintf(sz123,"G[%d]: %lf\n",n,G[n]);
	  // AfxMessageBox(sz123);                            
	  // sprintf(sz123,"dG[%d]: %lf\n",n,dG[n]);
	  // AfxMessageBox(sz123);                            
      // sprintf(sz123,"D2G[%d]: %lf\n",n,D2G[n]);
	  // AfxMessageBox(sz123);                            
        }
      else break;
    }
  filter_width=n;

  // AfxMessageBox("canny_core: smooth pic\n");
  /* allocate space for gaussian smoothing arrays */
  if ((gsmooth_x=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      AfxMessageBox("can't alloc gsmooth_x\n");
      return 0;
    }
  if ((gsmooth_y=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      AfxMessageBox("can't alloc gsmooth_y\n");
      return 0;
    }

  /* produce x- and y- convolutions with gaussian */

  ilimit=cols-(filter_width-1);
  jstart=cols*(filter_width-1);
  jlimit=cols*(rows-(filter_width-1));
  for (i=filter_width-1;i<ilimit;++i)
    {
      for(jfactor=jstart;
	  jfactor<jlimit;
	  jfactor+=cols)
        {
	  cindex=i+jfactor;
	  t1=data[cindex]*G[0];
	  t2=t1;
	  for(k=1,kfactor1=cindex-cols,
                kfactor2=cindex+cols;
	      k<filter_width;
	      k++,
                kfactor1-=cols,
                kfactor2+=cols)
            {
	      t1+=G[k]*(data[kfactor1]+
			data[kfactor2]);
	      t2+=G[k]*(data[cindex-k]+
			data[cindex+k]);
            }
	  gsmooth_x[cindex]=t1;
	  gsmooth_y[cindex]=t2;
        }
    }
    
  /* allocate space for gradient arrays */
  // AfxMessageBox("canny_core: find grad\n");
  if ((derivative_x=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      AfxMessageBox("can't alloc x\n");
      return 0;
    }
  /* produce x and y convolutions with derivative of
     gaussian */

  for (i=filter_width-1;i<ilimit;++i)
    {
      for(jfactor=jstart;
	  jfactor<jlimit;
	  jfactor+=cols)
        {
	  t1=0;
	  cindex=i+jfactor;
	  for (k=1;k<filter_width;++k)
	    t1+=dG[k]*(gsmooth_x[cindex-k]-
                       gsmooth_x[cindex+k]);
	  derivative_x[cindex]=t1;
        }
    }
  free(gsmooth_x);
  if ((derivative_y=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      AfxMessageBox("can't alloc y\n");
      return 0;
    }

  for (i=n;i<cols-n;++i)
    {
      for(jfactor=jstart;jfactor<jlimit;jfactor+=cols)
        {
	  t2=0;
	  cindex=i+jfactor;
	  for (k=1,kfactor=cols;
	       k<filter_width;
	       k++,kfactor+=cols)
	    t2+=dG[k]*(gsmooth_y[cindex-kfactor]-gsmooth_y[cindex+kfactor]);
	  derivative_y[cindex]=t2;
        }
    }
  free(gsmooth_y);
    
  /* non-maximum suppression (4 cases for orientation of line
     of max slope) */

  // AfxMessageBox("canny_core: non-maximum suppression\n");
  ilimit=cols-filter_width;
  jstart=cols*filter_width;
  jlimit=cols*(rows-filter_width);

  for (i=filter_width;i<ilimit;++i)
    {
      for (jfactor=jstart;
	   jfactor<jlimit;
	   jfactor+=cols)
        {
				/* calculate current indeces */
	  cindex=i+jfactor;
	  nindex=cindex-cols;
	  sindex=cindex+cols;
	  windex=cindex-1;
	  eindex=cindex+1;
	  nwindex=nindex-1;
	  neindex=nindex+1;
	  swindex=sindex-1;
	  seindex=sindex+1;
	  ux=derivative_x[cindex];
	  uy=derivative_y[cindex];
	  gc=Hypotenuse(ux,uy);
	  /* scale gc to fit into an unsigned char array */
	  t=gc*20.0;
/*AfxMessageBox("canny_core: i,j=(%d,%d), t=%lf\n",i,jfactor/cols,t);*/
	  derivative_mag[cindex]=(t<256 ? t : 255);
	  gn=Hypotenuse(derivative_x[nindex],derivative_y[nindex]);
	  gs=Hypotenuse(derivative_x[sindex],derivative_y[sindex]);
	  gw=Hypotenuse(derivative_x[windex],derivative_y[windex]);
	  ge=Hypotenuse(derivative_x[eindex],derivative_y[eindex]);
	  gne=Hypotenuse(derivative_x[neindex],derivative_y[neindex]);
	  gse=Hypotenuse(derivative_x[seindex],derivative_y[seindex]);
	  gsw=Hypotenuse(derivative_x[swindex],derivative_y[swindex]);
	  gnw=Hypotenuse(derivative_x[nwindex],derivative_y[nwindex]);
	  if (ux*uy>0)
            {
	      if(fabs(ux)<fabs(uy))
                {
		  if((g0=fabs(uy*gc))
                     < fabs(ux*gse+(uy-ux)*gs) ||
                     g0<=fabs(ux*gnw+(uy-ux)*gn))
                    continue;
                }
	      else
                {
		  if((g0=fabs(ux*gc))
                     < fabs(uy*gse+(ux-uy)*ge) ||
                     g0<=fabs(uy*gnw+(ux-uy)*gw))
                    continue;
                }
            }
	  else
            {
	      if(fabs(ux)<fabs(uy))
                {
		  if((g0=fabs(uy*gc))
                     < fabs(ux*gne-(uy+ux)*gn) ||
                     g0<=fabs(ux*gsw-(uy+ux)*gs))
		    continue;
                }
	      else
                {
		  if((g0=fabs(ux*gc))
                     < fabs(uy*gne-(ux+uy)*ge) ||
                     g0<=fabs(uy*gsw-(ux+uy)*gw))
		    continue;
                }
            }
	  /* seems to be a good scale factor */
	  magnitude[cindex]=derivative_mag[cindex];
	  /* pi*40 ~= 128 - direction is (thought
	     of as) a signed byte */
	  orientation[cindex]=atan2(uy, ux)*ORIENT_SCALE;
        }
    } 

  free(derivative_x);
  free(derivative_y);

	return 1;
}

int CCannyDetector::ThresholdingTracker(int high, int low, int cols, int rows, 
										unsigned char * data, 
										unsigned char * magnitude, 
										unsigned char * orientation)
{
    int i,j,k;    /* counters */
    int picsize;    /* picture area */

    // AfxMessageBox("thresholding_tracker: tracking edges, high=%d, low=%d\n",high,low);
    /* clear data array before tracking */
    picsize=cols*rows;
    for (i=0;i<picsize;++i) data[i]=0;

    /* threshold image with hysteresis: follow from
       each strong edge point */
    for (i=0;i<cols;++i)
    {
        for (j=0;j<rows;++j)
            if (magnitude[i+cols*j]>=high)
                Follow(i,j,low,cols,rows,data,magnitude,orientation);
    }

	return 1;
}

int CCannyDetector::Follow(int i, int j, int low, int cols, int rows, 
						   unsigned char * data, 
						   unsigned char * magnitude, 
						   unsigned char * orientation)
{
    int k,l;        /* counters */
    int i_plus_1,i_minus_1,j_plus_1,j_minus_1;
    long index,kindex;
    char break_flag;

    i_plus_1=i+1;
    i_minus_1=i-1;
    j_plus_1=j+1;
    j_minus_1=j-1;
    index=i+j*cols;
    if (j_plus_1>=rows) j_plus_1=rows-1;
    if (j_minus_1<0) j_minus_1=0;
    if (i_plus_1>=cols) i_plus_1=cols-1;
    if (i_minus_1<0) i_minus_1=0;
/*AfxMessageBox("follow: i,j=%d %d, i_plus_1,i_minus_1=%d %d\n",i,j,i_plus_1,i_minus_1);*/
    if (!data[index])
    {
/*AfxMessageBox("following %d %d\n",i,j);*/
        /* current point must be added to the list of tracked points */
        data[index]=magnitude[index];
        /* now we can check immediately adjacent points to see if
           they too could be added to the track */
        break_flag=0;
        for (k=i_minus_1;k<=i_plus_1;k++)
        {
            for(l=j_minus_1;l<=j_plus_1;++l)
            {
                kindex=k+l*cols;
                if (!(l==j && k==i) &&
                    magnitude[kindex]>=low/* &&
                    abs(abs(orientation[index]-orientation[kindex])-128)>120*/)
                {
                    if (Follow(k,l,low,cols,rows,data,magnitude,orientation))
                    {
                        break_flag=1;
                        break;
                    }
                }
            }
            if (break_flag) break;
        }
        return(1);
    }
    else return(0);

}

int CCannyDetector::EdgeDetect(int * imgData, int img_x, int img_y, int thresL, 
							   int thresU, float std, int* edgeData)
{

	// allocate data structures
    unsigned char *data;    /* input and output array */
    unsigned char *derivative_mag; /*mag of del G before non-maximum suppression*/
    unsigned char *magnitude,*orientation; /*mag & orient after non-max sppression*/
	int picsize = img_x * img_y;

    if ((data=(unsigned char *)malloc(picsize))==(unsigned char *)NULL)
    {
        AfxMessageBox("canny: can't alloc data");
        return 0;
    }
    if ((derivative_mag=(unsigned char *)calloc(picsize,1))==
        (unsigned char *)NULL)
    {
        AfxMessageBox("canny: can't alloc derivative_mag");
        return 0;
    }
    if ((magnitude=(unsigned char *)calloc(picsize,1))
        ==(unsigned char *)NULL)
    {
        AfxMessageBox("canny: can't alloc magnitude");
        return 0;
    }
    if ((orientation=(unsigned char *)calloc(picsize,1))
        ==(unsigned char *)NULL)
    {
        AfxMessageBox("canny: can't alloc orientation");
        return 0;
    }

	// copy input image to I/O buffer
	for(int i=0; i<picsize; i++)
		data[i] = imgData[i];
				 
	// detect edges
	CannyCore(std,img_x,img_y,data,derivative_mag,magnitude,orientation);
	ThresholdingTracker(thresU, thresL,img_x,img_y,data,magnitude,orientation);

	// copy buffer to edge image
	for(i=0; i<picsize; i++)
		edgeData[i] = data[i];

    free(magnitude);
    free(orientation);
	free(data);
	free(derivative_mag);

	return 1;
}
