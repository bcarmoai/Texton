// TemplateMatcher.cpp: implementation of the CTemplateMatcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "TemplateMatcher.h"
#include <math.h>
#include "numrecoptimiser.h"
#ifndef ANSI
#define ANSI
#endif
#include "nrutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTemplateMatcher::CTemplateMatcher()
{
	imgTmpl = NULL;
	errorImage = NULL;
	errorCanvas = 0;
	outputFile.Open("powell.txt", 
		CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
	sprintf(txtFinalParms,"");
	BuildCovarianceMatrix();
}

CTemplateMatcher::~CTemplateMatcher()
{
	if(imgTmpl!=NULL) iplDeallocate( imgTmpl, IPL_IMAGE_ALL );
	if(errorImage!=NULL) delete errorImage;
}

void CTemplateMatcher::match_main(int * imgData, int width, int height, 
								  void (*progress)(int), BOOL fixed_position, 
								  BOOL save_tofile, float* final_parms)
{
	float angle, lw, lh, w, g, wp;  // template parameters see log 30/01/02
	int iter;  // no. of iterations in Powell's method

	// deallocate memory
	if(imgTmpl!=NULL) iplDeallocate( imgTmpl, IPL_IMAGE_ALL );
	imgTmpl=NULL;
	iEvaluateCalls = 0;
	b_saveCalls = save_tofile;

	imgTmpl = CreateTmplImage(width, height);
	if(imgTmpl == NULL)
	{AfxMessageBox("Error creating template target image."); return;}

	srcImage = imgData;
	srcWidth = width;
	srcHeight = height;

	errorCanvas = (m_trainLW+m_trainWP)/2+5;

	if(m_trainLW<=m_trainLH || m_initLW<=m_initLH || m_initG>=m_initW)
	{AfxMessageBox("Condition(s) failed: m_trainLW<=m_trainLH || \
	m_initLW<=m_initLH || m_initG>=m_initW."); return;}

	if(fixed_position == FALSE)
	{
		if(errorImage!=NULL) delete errorImage;
		errorImage=NULL;
		errorImage = new float[width*height];
		for(int i=0;i<width*height;i++)
			errorImage[i]=0.0;

		if(m_templFixed == FALSE)
		{
			for(templ_xc=errorCanvas; templ_xc<(width-errorCanvas); templ_xc++)
				for(templ_yc=errorCanvas; templ_yc<(height-errorCanvas); templ_yc++)
				{
					iEvaluateCalls = 0;
					
					// optimise
					MinimisePowell(angle, lw, lh, w, g, wp, iter);
					
					// get error
					compareError = EvaluateTemplate(angle, lw, lh, w, g, wp);
					
					// draw error
					errorImage[width*templ_yc+templ_xc] = compareError;
					
					// report on progress
					DecodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
					sprintf(txtFinalParms, "(%i, %i) angle=%.0f lw=%.0f lh=%.0f w=%.0f g=%.0f wp=%.0f error = %f calls=%i",
						templ_xc, templ_yc, angle, lw, lh, w, g, wp, compareError, iEvaluateCalls);
					progress(int(ceil(100*(float)(height*templ_xc+templ_yc)/(float)(width*height))));
					
					if(b_saveCalls)
					{
						// output values
						CString s266 = (CString)txtFinalParms+(CString)"\n";
						outputFile.Write(s266, s266.GetLength());
					}
					
					
				}
		}
		else
		{
			
			// set initial values for parameters
			angle = m_trainANG; lw = m_trainLW; lh = m_trainLH; w=m_trainW; g=m_trainG; wp=m_trainWP;
			EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
			
			for(templ_xc=errorCanvas; templ_xc<(width-errorCanvas); templ_xc++)
				for(templ_yc=errorCanvas; templ_yc<(height-errorCanvas); templ_yc++)
				{
					iEvaluateCalls = 0;
					
					// get error
					compareError = EvaluateTemplate(angle, lw, lh, w, g, wp);
					
					// draw error
					errorImage[width*templ_yc+templ_xc] = compareError;
					
					// report on progress
					DecodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
					sprintf(txtFinalParms, "(%i, %i) angle=%.0f lw=%.0f lh=%.0f w=%.0f g=%.0f wp=%.0f error = %f calls=%i",
						templ_xc, templ_yc, angle, lw, lh, w, g, wp, compareError, iEvaluateCalls);
					progress(int(ceil(100*(float)(height*templ_xc+templ_yc)/(float)(width*height))));
					
					if(b_saveCalls)
					{
						// output values
						CString s266 = (CString)txtFinalParms+(CString)"\n";
						outputFile.Write(s266, s266.GetLength());
					}					
					
					EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
				}
		}
	}
	else
	{
		// optimise
		MinimisePowell(angle, lw, lh, w, g, wp, iter);
		
		// get error
		compareError = EvaluateTemplate(angle, lw, lh, w, g, wp);
		
		// report on progress
		DecodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
		sprintf(txtFinalParms, "angle=%.0f lw=%.0f lh=%.0f w=%.0f g=%.0f wp=%.0f error = %f calls=%i",
			angle, lw, lh, w, g, wp, compareError, iEvaluateCalls);
		progress(100.0);
		if(iEvaluateCalls>3000)
			sprintf(txtFinalParms, "aborted: iEvaluate calls > 3000");

		// angle, lw, lh, w, g, wp, error
		if(final_parms != NULL)
		{
			final_parms[0]=angle;
			final_parms[1]=lw;
			final_parms[2]=lh;
			final_parms[3]=w;
			final_parms[4]=g;
			final_parms[5]=wp;
			final_parms[6]=compareError;
		}
		
	}

}

BOOL CTemplateMatcher::DrawTemplate(IplImage * imgTmpl, int imgWidth, 
									int imgHeight, int x, int y, int angle, int lw, 
									int lh, int w, int g, int wp)
{
	// validate data
	if(lw<=0 || lw>20000 || lh<=0 || lh>20000 || wp <=0 || wp > 20000)
	{
		char sz266[100];
		sprintf(sz266, "Invalid parameters to DrawTemplate: lw=%i lh=%i wp=%i", lw, lh, wp);

		// AfxMessageBox(sz266);
		return FALSE;
	}

	// determine background colour to be different from w or g
	for(background = 0; background < 255; background++)
	{
		if(background != w && background != g)
			break;
	}
	iplSet(imgTmpl, background);

	// draw ellipse onto target
	CvPoint centerPoint;
	centerPoint.x = x;
	centerPoint.y = y;
	CvSize axesSize;
	axesSize.width = lw;
	axesSize.height = lh;
	cvEllipse( imgTmpl, centerPoint, axesSize, angle*(-1), 0, 360, w, 1 );

	// draw second ellipse onto target
	centerPoint.x = x;
	centerPoint.y = y;
	axesSize.width = lw+wp;
	axesSize.height = lh+wp;
	cvEllipse( imgTmpl, centerPoint, axesSize, angle*(-1), 0, 360, g, 1 );

	// fill ellipse points with g and w
	int xp, yp, yt, ytt;
	unsigned char buffer[4];

	for(xp=0; xp<imgWidth; xp++)
		for(yp=0; yp<imgHeight; yp++)
		{
			iplGetPixel(imgTmpl, xp, yp, &buffer);

			if(buffer[0] == w)
			{
				for(yt=yp+1; yt<imgHeight; yt++)
				{
					iplGetPixel(imgTmpl, xp, yt, &buffer);
					if(buffer[0]==w)
						break;
				}

				if(yt!=imgHeight)
				{
					for(ytt=yp+1; ytt<yt; ytt++)
						iplPutPixel(imgTmpl, xp, ytt, &w);

				}

				continue;
				
			}

			if(buffer[0] == g)
			{
				for(yt=yp+1; yt<imgHeight; yt++)
				{
					iplGetPixel(imgTmpl, xp, yt, &buffer);
					if(buffer[0]==g)
						break;
				}

				if(yt!=imgHeight)
				{
					for(ytt=yp+1; ytt<yt; ytt++)
					{
						iplGetPixel(imgTmpl, xp, ytt, &buffer);
						if(buffer[0]!=w)
							iplPutPixel(imgTmpl, xp, ytt, &g);
					}

				}


			}




		}

	return TRUE;
	
}

IplImage* CTemplateMatcher::CreateTmplImage(int width, int height)
{
	// allocate ellipse target image
	IplImage *imgTempl = NULL;
	imgTempl = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		width, height, NULL, NULL, NULL, NULL );
	if( !imgTempl ) return NULL;
	iplAllocateImage( imgTempl, 0, 0 );
	if( !imgTempl->imageData ) return NULL;

	return imgTempl;

}

void CTemplateMatcher::EncodeParameters(float& angle, float& lw, 
										float& lh, float& w, float& g, float& wp,
										float& train_wp, float& train_lh, float& train_lw)
{
//	angle = log(angle);
	angle = (angle-90.0)/0.00034;
	lw = log(lw-lh);
//	lw= log(lw);
	lh = log(lh);
	w = log(w-g);
	g = log(g);
	wp = log(wp-0.51f);
	train_wp = log(train_wp);
	train_lw = log(train_lw-train_lh);
	train_lh = log(train_lh);

}

void CTemplateMatcher::DecodeParameters(float& angle, float& lw, 
										float& lh, float& w, float& g, float& wp,
										float& train_wp, float& train_lh, float& train_lw)
{
//	angle = exp(angle);
	angle = 0.00034*angle+90.0;
	lh = exp(lh);
	lw = lh + exp(lw);
//	lw = exp(lw);
	g = exp(g);
	w = g + exp(w);
	wp = 0.51f + exp(wp);
	train_wp=exp(train_wp);
	train_lh=exp(train_lh);	
	train_lw=train_lh+exp(train_lw);
}

CTemplateMatcher* pCbckTmplMatcher;

float callback_CTemplateMatcher(float args[])
{
	return pCbckTmplMatcher->EvaluateTemplate(args[1], args[2], args[3], args[4],
		args[5], args[6]);
}

float CTemplateMatcher::PowellTester(float x [ ])
{

	float toret;
	toret = 0.5-_j0(pow(x[1]-1.0,2)+pow(x[2]-2.0,2)+pow(x[3]-3.0,2));

	// output values
	char sz266[200]={0};
	sprintf(sz266, "%f %f %f %f\n", x[1], x[2], x[3], toret);
	CString s266 = sz266;
	outputFile.Write(s266, s266.GetLength());

	return toret;

}

float callback_bessel(float x[])
{
	return pCbckTmplMatcher->PowellTester(x);
}

void CTemplateMatcher::MinimisePowell(float & angle, float & lw, float & lh, 
									  float & w, float & g, float & wp, int & iter)
{
    const int NDIM = 6;
	CNumrecOptimiser numRec;

	// set initial values for parameters
	angle = iInitAngle; lw = m_initLW; lh = m_initLH; w=m_initW; g=m_initG; wp=m_initWP;
	EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);

	// prepare data structures - note 1-based indexes in num. recipes' code
	float p[] = {0.0, angle, lw, lh, w, g, wp};   // initial position vector
	float** xi;    // initial directions matrix
	int n = NDIM;			// number of variables
	float ftol = 1.0e-6;   // fractional tolerance around minimum
	float fret;			// to receive function final value

	xi = matrix(1,NDIM,1,NDIM);
	int i,j;
	for(i=1; i<=NDIM; i++)
		for(j=1; j<=NDIM; j++)
			xi[i][j]=0.0;
	xi[1][1]=m_initAMAG;
	xi[2][2]=m_initLWMAG;
	xi[3][3]=m_initLHMAG;
	xi[4][4]=m_initWMAG;
	xi[5][5]=m_initGMAG;
	xi[6][6]=m_initWPMAG;

	pCbckTmplMatcher = this;

	// call test function
	// set NDIM to 3
	/*
	float pbessel[] = {0.0,1.5,1.5,2.5};
	numRec.powell(pbessel, xi, n, ftol, &iter, &fret, callback_bessel);
	angle=p[1];lw=p[2];lh=p[3];w=p[4];g=p[5];wp=p[6];
	angle=pbessel[1];lw=pbessel[2];lh=pbessel[3];
	w=pbessel[4];g=pbessel[5];wp=pbessel[6];
	*/

	// call function
	// set NDIM to 6
	numRec.powell(p, xi, n, ftol, &iter, &fret, callback_CTemplateMatcher);
	angle=p[1];lw=p[2];lh=p[3];w=p[4];g=p[5];wp=p[6];

	free_matrix(xi,1,NDIM,1,NDIM);

}

float callback_simplextest(float x[])
{
	return pCbckTmplMatcher->SimplexTester(x);
}

// minimises analytical function in SimplexTest()
void CTemplateMatcher::MinimiseSimplexTest()
{
	const int NP = 3, MP = 4;
	const float FTOL = 1.0e-6;
	int i,j,nfunc,ndim=NP;
	float *x,*y,**p;

	CNumrecOptimiser numRec;
	pCbckTmplMatcher = this;

	x=vector(1,NP);
	y=vector(1,MP);
	p=matrix(1,MP,1,NP);
	for(i=1;i<=MP;i++)
	{
		for(j=1;j<=NP;j++)
			x[j]=p[i][j]=(i==(j+1)?1.0:0.0);
		y[i]=callback_simplextest(x);
	}

	numRec.amoeba(p,y,ndim,FTOL,callback_simplextest,&nfunc);

	// loop below is so we can inspect function values with debugger
	for(i=1;i<=MP;i++)
	{
		for(j=1;j<=NP;j++)
		{
			p[i][j];
		}
		
		y[i];
	}

	free_matrix(p,1,MP,1,NP);
	free_vector(y,1,MP);
	free_vector(x,1,NP);
}

void CTemplateMatcher::MinimiseSimplex(float & angle, float & lh, float & lw, 
									   float & w, float & g, float & wp, int & iter)
{
	const int NP = 6, MP = 7;
	const float FTOL = 1.0e-6;
	int i,j,nfunc,ndim=NP;
	float *x,*y,**p;

	CNumrecOptimiser numRec;
	pCbckTmplMatcher = this;

	x=vector(1,NP);
	y=vector(1,MP);
	p=matrix(1,MP,1,NP);

	for(i=1;i<=MP;i++)
		for(j=1;j<=NP;j++)
			p[i][j]=0.0;

	angle=90; lw=25; lh=20; w=250; g=150; wp=9;
	EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);

	p[1][1]=angle;
	p[2][2]=lw;
	p[3][3]=lh;
	p[4][4]=w;
	p[5][5]=g;
	p[6][6]=wp;

	for(i=1;i<=MP;i++)
	{
		for(j=1;j<=NP;j++)
			x[j]=p[i][j];
		y[i]=callback_CTemplateMatcher(x);
	}

	numRec.amoeba(p,y,ndim,FTOL,callback_CTemplateMatcher,&nfunc);

	FILE* toOut = fopen("test2.txt", "w");
	fprintf(toOut,"");
	fprintf(toOut,"Number of function evaluations: %3d\n",nfunc);
	fprintf(toOut,"Vertices of final 3-d simplex and\n");
	fprintf(toOut,"function values at the vertices:\n");
	fprintf(toOut,"%3s %11s %11s %11s %12s %12s %12s %14s\n\n","i",
		"an[i]","lw[i]","lh[i]","w[i]","g[i]","wp[i]","function");
	for(i=1;i<=MP;i++)
	{
		fprintf(toOut,"%3d", i);
		for(j=1;j<=NP;j++) fprintf(toOut,"%12.6f",p[i][j]);
		fprintf(toOut,"%12.6f\n",y[i]);
	}

	DecodeParameters(p[1][1],p[1][2],p[1][3],p[1][4],p[1][5],p[1][6], m_trainWP, m_trainLH, m_trainLW);
	fprintf(toOut,"\nDecoded line 1: %f %f %f %f %f %f \n",
		p[1][1],p[1][2],p[1][3],p[1][4],p[1][5],p[1][6]);

	fclose(toOut);

	free_matrix(p,1,MP,1,NP);
	free_vector(y,1,MP);
	free_vector(x,1,NP);
}

float CTemplateMatcher::SimplexTester(float x [ ])
{
	float toret;
	toret = 0.6-_j0(pow(x[1]-0.5,2)+pow(x[2]-0.6,2)+pow(x[3]-0.7,2));

	// output values
	char sz266[200]={0};
	sprintf(sz266, "%f %f %f %f\n", x[1], x[2], x[3], toret);
	CString s266 = sz266;
	outputFile.Write(s266, s266.GetLength());

	return toret;

}

float CTemplateMatcher::EvaluateTemplate(float angle, float lw, float lh, float w, 
										 float g, float wp, BOOL bEncoded)
{
	const float LARGE_ERROR = 5000.0;
	float error;
	int width = srcWidth;
	int height = srcHeight;

	// translate parameter values
	if(bEncoded)
		DecodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);

	// convert parameters to integers
	int iangle, ilw, ilh, iw, ig, iwp;
	double dmy;
	iangle = (modf(angle,&dmy)>0.5?ceil(angle):floor(angle));
	ilw = (modf(lw,&dmy)>0.5?ceil(lw):floor(lw));
	ilh = (modf(lh,&dmy)>0.5?ceil(lh):floor(lh));
	iw = (modf(w,&dmy)>0.5?ceil(w):floor(w));
	ig = (modf(g,&dmy)>0.5?ceil(g):floor(g));
	iwp = (modf(wp,&dmy)>0.5?ceil(wp):floor(wp));

	// output values
	if(b_saveCalls)
	{
		char sz266[200]={0};
		sprintf(sz266, "%i %i %i %i %i %i", iangle, ilw, ilh, iw, ig, iwp);
		CString s266 = sz266;
		outputFile.Write(s266, s266.GetLength());
	}
	
	if(iEvaluateCalls > 3000)
	{
		if(b_saveCalls)
		{
			outputFile.Write("\n", 1);
		}
		if(bEncoded)
			EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
		return LARGE_ERROR;
	}

	// generate template image, uses class members tmpl_xc, templ_yc set by current loop
	if(DrawTemplate(imgTmpl, width, height,
		templ_xc, templ_yc, iangle, ilw, ilh, iw, ig, iwp)==FALSE)
	{
		if(bEncoded)
			EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
		return LARGE_ERROR;
	}

	// derive error
	int x, y, T;
	unsigned char buffer[4];
	T=0;
	error = 0.0;

	// get template bounding rectangle
	// get total number of pixels
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			iplGetPixel(imgTmpl, x, y, &buffer);
			if(buffer[0] != background)
				T++; // increase area of template by one pixel
		}

	// get average difference between template and image pixels
	int pTemp;
	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			iplGetPixel(imgTmpl, x, y, &buffer);
			pTemp=srcImage[width*y+x];
			if(buffer[0] != background)
				error = error + m_weightINT*pow((buffer[0]-srcImage[width*y+x]), 2.0) / (double)(T!=0?T:1);

		}

	// wp term
	error = error + m_weigthWP*pow(wp-m_trainWP, 2);

	// lw term
	error = error + m_weigthLW*pow(lw-m_trainLW, 2);

	// lh term
	error = error + m_weigthLH*pow(lh-m_trainLH, 2);

	// penalise w and g over 255
	if(w>255 || g>255)
		error = error + 10000;

	// add mahalanobis distance to trained parameters
	float obs_vector[COV_PARMS] = {lw, lh, wp, w, g};
	double mahal_error;

    CvMat matCovar = cvMat(COV_PARMS, COV_PARMS, CV_MAT32F, covariance_matrix);
	CvMat matObsrv = cvMat(COV_PARMS, 1, CV_MAT32F, obs_vector);
	CvMat matMeanv = cvMat(COV_PARMS, 1, CV_MAT32F, mean_vector_A);
	
	mahal_error = cvMahalanobis( &matObsrv, &matMeanv, &matCovar );
	// cvMahalanobis() is implemented in file opencv\cv\src\cvamatmul.cpp

	/*
	// check result with manual ops
		// allocate memory
	double error_check;
	CvMat sub, subT, cinv, mprod, fprod;
	sub = cvMat(COV_PARMS, 1, CV_MAT32F, 0);
	subT = cvMat(1, COV_PARMS, CV_MAT32F, 0);
	cinv = cvMat(COV_PARMS, COV_PARMS, CV_MAT32F, 0);
	mprod = cvMat(1, COV_PARMS, CV_MAT32F, 0);
	fprod = cvMat(1, 1, CV_MAT32F, 0);
	cvmAlloc(&sub);
	cvmAlloc(&subT);
	cvmAlloc(&cinv);
	cvmAlloc(&mprod);
	cvmAlloc(&fprod);
		// get (theta-mean)
	cvmSub(&matObsrv, &matMeanv, &sub);
	    // get (theta-mean)^T
	cvmTranspose(&sub, &subT);
		// get inverse of covariance matrix
	cvmInvert(&matCovar, &cinv);
		// get first product
	cvmMul(&subT, &cinv, &mprod);
		// get second product
	cvmMul(&mprod, &sub, &fprod);
	error_check = fprod.data.fl[0];
	*/

	error = error + m_weigthMH*mahal_error;		

	if(b_saveCalls)
	{
		char sz266[200]={0};
		sprintf(sz266, " %f\n", error);
		CString s266 = sz266;
		outputFile.Write(s266, s266.GetLength());
	}

	if(bEncoded)
		EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);

	iEvaluateCalls++;

	return error;

}


void CTemplateMatcher::DrawInitial(int * imgData, int width, int height, 
								  void (*progress)(int))
{
	float angle, lw, lh, w, g, wp;  // template parameters see log 30/01/02
	float compareError;   // errors between image and template
	int iter;  // no. of iterations in Powell's method

	// deallocate memory
	if(imgTmpl!=NULL) iplDeallocate( imgTmpl, IPL_IMAGE_ALL );
	imgTmpl=NULL;
	if(errorImage!=NULL) delete errorImage;
	errorImage=NULL;
	iEvaluateCalls = 0;

	imgTmpl = CreateTmplImage(width, height);
	if(imgTmpl == NULL)
	{AfxMessageBox("Error creating template target image."); return;}
	errorImage = new float[width*height];

	srcImage = imgData;
	srcWidth = width;
	srcHeight = height;

	// get error
	angle = iInitAngle;
	g = m_initG;
	lh = m_initLH;
	lw = m_initLW;
	wp = m_initWP;
	w= m_initW;

	EncodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
	compareError = EvaluateTemplate(angle, lw, lh, w, g, wp);
	
	// report results
	DecodeParameters(angle, lw, lh, w, g, wp, m_trainWP, m_trainLH, m_trainLW);
	sprintf(txtFinalParms, "angle=%.0f lw=%.0f lh=%.0f w=%.0f g=%.0f wp=%.0f error = %f calls=%i",
		angle, lw, lh, w, g, wp, compareError, iEvaluateCalls);

}

void CTemplateMatcher::BuildCovarianceMatrix()
{
	const int EXPLS_A = 10;

	const float examples_vector_A[COV_PARMS][EXPLS_A]= /* lw, lh, wp, w, g */
		/* note each bracketed set contains values for each feature */
		{/*lw*/{5,5,5,9,9,6,5,5,6,8},
	     /*lh*/{5,4,4,6,4,5,4,3,4,5},
		 /*wp*/{6,7,9,3,5,5,5,5,5,5},
	     /*g */{226,214,236,243,245,222,225,213,245,227},
	     /*h */{128,121,139,132,141,124,132,119,143,144}};

	// build mean vector
	int parm, sample;
	for(parm=0; parm<COV_PARMS; parm++)
		mean_vector_A[parm] = 0.0f;
	for(parm=0; parm<COV_PARMS; parm++)
		for(sample=0; sample<EXPLS_A; sample++)
			mean_vector_A[parm] = mean_vector_A[parm] +
				(examples_vector_A[parm][sample]/(float)EXPLS_A);

	// build covariance matrix
	int parm_i, parm_j;
	for(parm_i=0; parm_i<COV_PARMS; parm_i++)
		for(parm_j=0; parm_j<COV_PARMS; parm_j++)
			covariance_matrix[parm_i][parm_j] = 0.0f;
	for(parm_i=0; parm_i<COV_PARMS; parm_i++)
		for(parm_j=0; parm_j<COV_PARMS; parm_j++)
			for(sample=0; sample<EXPLS_A; sample++)
				covariance_matrix[parm_i][parm_j] = covariance_matrix[parm_i][parm_j] +
					(1.0f/((float)EXPLS_A))*
					(examples_vector_A[parm_i][sample]-mean_vector_A[parm_i])*
					(examples_vector_A[parm_j][sample]-mean_vector_A[parm_j]);

	return;
}
