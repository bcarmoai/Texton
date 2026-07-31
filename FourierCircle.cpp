// FourierCircle.cpp: implementation of the CFourierCircle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "FourierCircle.h"
#include <math.h>
#include "nrutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define OPENCV TRUE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFourierCircle::CFourierCircle()
{

}

CFourierCircle::~CFourierCircle()
{

}

void CFourierCircle::SetCanny(int * img_canny, int img_width, int img_height)
{
	imgHeight = img_height;
	imgWidth = img_width;

	imgCanny = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		img_width, img_height, NULL, NULL, NULL, NULL );
	if( !imgCanny ) return;
	iplAllocateImage( imgCanny, 0, 0 );
	if( !imgCanny->imageData ) return;
			
	// copy source
	int x,y, buffer;
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			buffer = img_canny[imgWidth*y+x];
			iplPutPixel(imgCanny, x, y, &buffer);
		}
}

void CFourierCircle::ResetData()
{
	iplDeallocate( imgCanny, IPL_IMAGE_ALL );
	iplDeallocate( imgFFT, IPL_IMAGE_ALL );
	iplDeallocate( imgRotatedFFT, IPL_IMAGE_ALL );
	iplDeallocate( imgInvFFT, IPL_IMAGE_ALL );

}

void CFourierCircle::StepRotate(int angle)
{
	// allocate target
	imgRotated = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgRotated ) return;
	iplAllocateImage( imgRotated, 0, 0 );
	if( !imgRotated->imageData ) return;

	// rotate
	double xshift, yshift;
	iplGetRotateShift(0, imgHeight-1, (float)angle, &xshift, &yshift);
	iplRotate(imgCanny, imgRotated, (float)angle, xshift, yshift, IPL_INTER_NN);

}

void CFourierCircle::GetDataFFT(int* img_fourier/*, int width, int height*/)
{
	
	if(OPENCV)
	{
		
		// allocate conversion dummy
		IplImage* dummy;
		dummy = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
			IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
			imgWidth, imgHeight, NULL, NULL, NULL, NULL );
		if( !dummy ) return;
		iplAllocateImage( dummy, 0, 0 );
		if( !dummy->imageData ) return;
		
		iplConvert(imgFFT, dummy);
		
		int x, y;
		unsigned char buffer[4];
		for(x=0; x<imgWidth; x++)
			for(y=0; y<imgHeight; y++)
			{	
				iplGetPixel(dummy, x, y, &buffer);
				img_fourier[imgWidth*y+x] = buffer[0];
			}
	}
/*	else
	{
		*img_fourier = new int[squareWidth*squareHeight];
		*width = squareWidth;
		*height = squareHeight;	
			
		int x, y;

		for(x=0; x<squareWidth; x++)
			for(y=0; y<squareHeight; y++)
				*img_fourier[squareWidth*y+x] = imgNRfftReal[squareWidth*y+x];
	
	}
*/
}

void save_array_float(char* name, float* array, int width, int height)
{
	FILE* pFile;

	pFile = fopen(name, "w");
	if(pFile==NULL)
		AfxMessageBox("File not opened");

	int x, y;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
			fprintf(pFile, "%f\t", array[width*y+x]);
		
		fprintf(pFile, "\n");

	}

	fclose(pFile);

}

void save_NRarray_float(char* name, float* array, int width, int height)
{
	FILE* pFile;

	pFile = fopen(name, "w");
	if(pFile==NULL)
		AfxMessageBox("File not opened");

	int x, y;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
			fprintf(pFile, "%f\t", array[width*y+x]);
		
		fprintf(pFile, "\n");

	}

	fclose(pFile);

}


void CFourierCircle::StepFFT()
{

	// allocate target
	imgFFT = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgFFT ) return;
	iplAllocateImage( imgFFT, 0, 0 );
	if( !imgFFT->imageData ) return;
	
	if(OPENCV)
	{
		iplRealFft2D(imgCanny, imgFFT, IPL_FFT_Forw);
	}
	else
	{
		// numerical recipes version
		int* image = new int[imgWidth*imgHeight];
		int x, y;
		unsigned char buffer[4];
		for(x=0; x<imgWidth; x++)
			for(y=0; y<imgHeight; y++)
			{
				iplGetPixel(imgCanny, x, y, &buffer);
				image[imgWidth*y+x] = buffer[0];
			}
			
		NRfournCaller(image, &imgNRfftReal, &imgNRfftImaginary, imgWidth, imgHeight, 1);
			
		int buffer2;
		for(x=0; x<imgWidth; x++)
			for(y=0; y<imgHeight; y++)
			{	
				buffer2 = abs(imgNRfftReal[imgWidth*y+x])*10;
				iplPutPixel(imgFFT, x, y, &buffer2);
			}
			
		delete image;

		save_array_float("freal.txt", imgNRfftReal, imgWidth, imgHeight);
		save_array_float("fimag.txt", imgNRfftImaginary, imgWidth, imgHeight);

	}


}

void CFourierCircle::GetCanny(int * img_canny)
{
	int x, y;
	unsigned char buffer[4];
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{
			iplGetPixel(imgCanny, x, y, &buffer);
			img_canny[imgWidth*y+x] = buffer[0];
		}

}

void  save_array_int(char* name, int* array, int width, int height)
{
	FILE* pFile;

	pFile = fopen(name, "w");
	if(pFile==NULL)
		AfxMessageBox("File not opened");

	int x, y;

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
			fprintf(pFile, "%i\t", array[width*y+x]);
		
		fprintf(pFile, "\n");

	}

	fclose(pFile);

}

void CFourierCircle::InverseFFT()
{
	// allocate target
	imgInvFFT = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgInvFFT ) return;
	iplAllocateImage( imgInvFFT, 0, 0 );
	if( !imgInvFFT->imageData ) return;

	if(OPENCV)
	{
		iplCcsFft2D(imgFFT, imgInvFFT, IPL_FFT_Inv);
	}
	else
	{
		// numerical recipes version
		int* image = new int[imgWidth*imgHeight];
	
		NRfournCaller(image, &imgNRfftReal, &imgNRfftImaginary, imgWidth, imgHeight, -1);
			
		int buffer2, x, y;
		for(x=0; x<imgWidth; x++)
			for(y=0; y<imgHeight; y++)
			{	
				buffer2 = image[imgWidth*y+x];
				iplPutPixel(imgInvFFT, x, y, &buffer2);
			}

		save_array_int("image.txt", image, imgWidth, imgHeight);

		delete image;

	}

}

void CFourierCircle::GetDataInvFFT(int * image)
{
	// allocate conversion dummy
	IplImage* dummy;
	dummy = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !dummy ) return;
	iplAllocateImage( dummy, 0, 0 );
	if( !dummy->imageData ) return;

	iplConvert(imgInvFFT, dummy);

	int x, y;
	unsigned char buffer[4];
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			iplGetPixel(dummy, x, y, &buffer);
			image[imgWidth*y+x] = buffer[0];
		}

}

void CFourierCircle::GetDataRotated(int * image)
{
	int x, y;
	unsigned char buffer[4];
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			iplGetPixel(imgRotated, x, y, &buffer);
			image[imgWidth*y+x] = buffer[0];
		}
}

void CFourierCircle::SetFFT(int * image, int width, int height)
{
	imgWidth = width;
	imgHeight = height;

	// allocate target
	imgFFT = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgFFT ) return;
	iplAllocateImage( imgFFT, 0, 0 );
	if( !imgFFT->imageData ) return;

	// copy source
	int x,y, buffer;
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			buffer = image[imgWidth*y+x];
			iplPutPixel(imgFFT, x, y, &buffer);
		}
}

void CFourierCircle::StepFFTRotated()
{
	// allocate target
	imgRotatedFFT = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgRotatedFFT ) return;
	iplAllocateImage( imgRotatedFFT, 0, 0 );
	if( !imgRotatedFFT->imageData ) return;

	// compute FFT
	iplRealFft2D(imgRotated, imgRotatedFFT, IPL_FFT_Forw);

}

void CFourierCircle::GetDataFFTRotated(int* img_fourier)
{
	// allocate conversion dummy
	IplImage* dummy;
	dummy = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !dummy ) return;
	iplAllocateImage( dummy, 0, 0 );
	if( !dummy->imageData ) return;

	iplConvert(imgRotatedFFT, dummy);

	int x, y;
	unsigned char buffer[4];
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			iplGetPixel(dummy, x, y, &buffer);
			img_fourier[imgWidth*y+x] = buffer[0];
		}

}

void CFourierCircle::InverseRotatedFFT()
{
	// allocate target
	imgRotatedInverse = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgRotatedInverse ) return;
	iplAllocateImage( imgRotatedInverse, 0, 0 );
	if( !imgRotatedInverse->imageData ) return;

	// compute inverse FFT
	iplCcsFft2D(imgRotatedFFT, imgRotatedInverse, IPL_FFT_Inv);


}

void CFourierCircle::GetDataInvRotatedFFT(int* image)
{
	// allocate conversion dummy
	IplImage* dummy;
	dummy = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !dummy ) return;
	iplAllocateImage( dummy, 0, 0 );
	if( !dummy->imageData ) return;

	iplConvert(imgRotatedInverse, dummy);

	int x, y;
	unsigned char buffer[4];
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			iplGetPixel(dummy, x, y, &buffer);
			image[imgWidth*y+x] = buffer[0];
		}

}

void CFourierCircle::StepCombine()
{
	// allocate combined image
	imgCombined = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgCombined ) return;
	iplAllocateImage( imgCombined, 0, 0 );
	if( !imgCombined->imageData ) return;
	
	// allocate rotated (dummy) image
/*	IplImage* dummy = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !dummy ) return;
	iplAllocateImage( dummy, 0, 0 );
	if( !dummy->imageData ) return;
*/
	// iplMirror(imgRotatedFFT, dummy, -1);

	iplMultiply(imgFFT, imgRotatedFFT, imgCombined);


}

void CFourierCircle::GetDataCombined(int* image)
{
	// allocate conversion dummy
	IplImage* dummy;
	dummy = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !dummy ) return;
	iplAllocateImage( dummy, 0, 0 );
	if( !dummy->imageData ) return;

	iplConvert(imgCombined, dummy);

	int x, y;
	unsigned char buffer[4];
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			iplGetPixel(dummy, x, y, &buffer);
			image[imgWidth*y+x] = buffer[0];
		}

}

void CFourierCircle::StepInvert()
{
	// allocate target
	imgInverted = iplCreateImageHeader(1,0,IPL_DEPTH_32S,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !imgInverted ) return;
	iplAllocateImage( imgInverted, 0, 0 );
	if( !imgInverted->imageData ) return;

	// compute inverse FFT
	iplCcsFft2D(imgCombined, imgInverted, IPL_FFT_Inv);

}

void CFourierCircle::GetDataInverted(int * image)
{
	// allocate conversion dummy
	IplImage* dummy;
	dummy = iplCreateImageHeader(1,0,IPL_DEPTH_8U,"GRAY","GRAY", 
		IPL_DATA_ORDER_PIXEL, IPL_ORIGIN_TL, IPL_ALIGN_DWORD,
		imgWidth, imgHeight, NULL, NULL, NULL, NULL );
	if( !dummy ) return;
	iplAllocateImage( dummy, 0, 0 );
	if( !dummy->imageData ) return;

	iplConvert(imgInverted, dummy);

	int x, y;
	unsigned char buffer[4];
	for(x=0; x<imgWidth; x++)
		for(y=0; y<imgHeight; y++)
		{	
			iplGetPixel(dummy, x, y, &buffer);
			image[imgWidth*y+x] = buffer[0];
		}

}

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
void CFourierCircle::NRfourn(float data [ ], unsigned long nn [ ], int ndim, int isign)
{

	// from Numerical Recipes

	int idim;
	unsigned long i1,i2,i3,i2rev,i3rev,ip1,ip2,ip3,ifp1,ifp2;
	unsigned long ibit,k1,k2,n,nprev,nrem,ntot;
	float tempi,tempr;
	double theta,wi,wpi,wpr,wr,wtemp; 
	for (ntot=1,idim=1;idim<=ndim;idim++) 
		ntot *= nn[idim];
	nprev=1;
	for (idim=ndim;idim>=1;idim--) 
	{
		n=nn[idim];
		nrem=ntot/(n*nprev);
		ip1=nprev << 1;
		ip2=ip1*n;
		ip3=ip2*nrem;
		i2rev=1;
		for (i2=1;i2<=ip2;i2+=ip1) 
		{ 
			if (i2 < i2rev) {
				for (i1=i2;i1<=i2+ip1-2;i1+=2) 
				{
					for (i3=i1;i3<=ip3;i3+=ip2) 
					{
						i3rev=i2rev+i3-i2;
						SWAP(data[i3],data[i3rev]);
						SWAP(data[i3+1],data[i3rev+1]);
					}
				}
			}
			ibit=ip2 >> 1;
			while (ibit >= ip1 && i2rev > ibit) 
			{
				i2rev -= ibit;
				ibit >>= 1;
			}
			i2rev += ibit;
		}
		ifp1=ip1; 
		while (ifp1 < ip2) 
		{
			ifp2=ifp1 << 1;
			theta=isign*6.28318530717959/(ifp2/ip1); 
			wtemp=sin(0.5*theta);
			wpr = -2.0*wtemp*wtemp;
			wpi=sin(theta);
			wr=1.0;
			wi=0.0;
			for (i3=1;i3<=ifp1;i3+=ip1) 
			{
				for (i1=i3;i1<=i3+ip1-2;i1+=2) 
				{
					for (i2=i1;i2<=ip3;i2+=ifp2) 
					{
						k1=i2; 
						k2=k1+ifp1;
						tempr=(float)wr*data[k2]-(float)wi*data[k2+1];
						tempi=(float)wr*data[k2+1]+(float)wi*data[k2];
						data[k2]=data[k1]-tempr;
						data[k2+1]=data[k1+1]-tempi;
						data[k1] += tempr;
						data[k1+1] += tempi;
					}
				}
				wr=(wtemp=wr)*wpr-wi*wpi+wr; 
				wi=wi*wpr+wtemp*wpi+wi;
			}
			ifp1=ifp2;
		}
		nprev *= n;
	}

}

void CFourierCircle::NRrlft3(float * * * data, float * * speq, unsigned long nn1, unsigned long nn2, unsigned long nn3, int isign)
{
	unsigned long i1,i2,i3,j1,j2,j3,nn[4],ii3;
	double theta,wi,wpi,wpr,wr,wtemp;
	float c1,c2,h1r,h1i,h2r,h2i;
	if (1+&data[nn1][nn2][nn3]-&data[1][1][1] != nn1*nn2*nn3)
		AfxMessageBox("rlft3: problem with dimensions or contiguity of data array\n");
	c1=0.5;
	c2 = -0.5*isign;
	theta=isign*(6.28318530717959/nn3);
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	nn[1]=nn1;
	nn[2]=nn2;
	nn[3]=nn3 >> 1;
	if (isign == 1) { 
		NRfourn(&data[1][1][1]-1,nn,3,isign); 
		for (i1=1;i1<=nn1;i1++)
			for (i2=1,j2=0;i2<=nn2;i2++) { 
				speq[i1][++j2]=data[i1][i2][1];
				speq[i1][++j2]=data[i1][i2][2];
			}
	}
	for (i1=1;i1<=nn1;i1++) {
		j1=(i1 != 1 ? nn1-i1+2 : 1);
		wr=1.0;
		wi=0.0;
		for (ii3=1,i3=1;i3<=(nn3>>2)+1;i3++,ii3+=2) {
			for (i2=1;i2<=nn2;i2++) {
				if (i3 == 1) { 
					j2=(i2 != 1 ? ((nn2-i2)<<1)+3 : 1);
					h1r=c1*(data[i1][i2][1]+speq[j1][j2]);
					h1i=c1*(data[i1][i2][2]-speq[j1][j2+1]);
					h2i=c2*(data[i1][i2][1]-speq[j1][j2]);
					h2r= -c2*(data[i1][i2][2]+speq[j1][j2+1]);
					data[i1][i2][1]=h1r+h2r;
					data[i1][i2][2]=h1i+h2i;
					speq[j1][j2]=h1r-h2r;
					speq[j1][j2+1]=h2i-h1i;
				} else {
					j2=(i2 != 1 ? nn2-i2+2 : 1);
					j3=nn3+3-(i3<<1);
					h1r=c1*(data[i1][i2][ii3]+data[j1][j2][j3]);
					h1i=c1*(data[i1][i2][ii3+1]-data[j1][j2][j3+1]);
					h2i=c2*(data[i1][i2][ii3]-data[j1][j2][j3]);
					h2r= -c2*(data[i1][i2][ii3+1]+data[j1][j2][j3+1]);
					data[i1][i2][ii3]=h1r+wr*h2r-wi*h2i;
					data[i1][i2][ii3+1]=h1i+wr*h2i+wi*h2r;
					data[j1][j2][j3]=h1r-wr*h2r+wi*h2i;
					data[j1][j2][j3+1]= -h1i+wr*h2i+wi*h2r;
				}
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
	}
	if (isign == -1)
		NRfourn(&data[1][1][1]-1,nn,3,isign);

}

float GetNrArrayElement(float* array, int width, int height, int x, int y)
{
	// width and height are the real width and height, x and y are zero-based
	return (array+1)[(width-1)*(y+1)+(x+1)];
}

void CFourierCircle::NRfournCaller(int* image, float** real, float** imaginary, int width, int height, int forward)
{
	// to call: 1 for forward, -1 for inverse

	if(forward==1)
	{
		// we need to padd the image with zeroes to make for 
		// sizes that are powers of two (an NR requirement)
		int xn;
		for(xn=1; xn<15; xn++)
			if(pow(2.0, xn)>=width)
				break;
		
		int yn;
		for(yn=1; yn<15; yn++)
			if(pow(2.0, yn)>=height)
				break;
			
		int dataArrWidth, dataArrHeight;
			
		dataArrWidth = (int)pow(2.0, xn) * 2;
		dataArrHeight = (int)pow(2.0, yn);

		squareWidth = (int)pow(2.0, xn);
		squareHeight = (int)pow(2.0, yn);

		*real = new float[squareWidth*squareHeight];
		*imaginary = new float[squareWidth*squareHeight];
	
		// allocate the bigger image with the new sizes
		float* transform = new float[dataArrWidth*dataArrHeight+1];
		int x, y;
			
		// fill the data with  zeros
		for(x=0; x<=dataArrWidth*dataArrHeight; x++)
			transform[x] = 0;
		
		for(x=0; x<width; x++)
			for(y=0; y<height; y++)
				transform[dataArrWidth*(y+1)+2*(x+1)]=image[width*y+x];
			
		// dimensions vector
		unsigned long nn[2];
		nn[1]=(int)pow(2.0, xn);
		nn[2]=(int)pow(2.0, yn);
		
		NRfourn((transform+1), nn, 2, 1);
//		NRfourn((transform+1), nn, 2, -1);
		save_array_float("farrayF.txt", transform+1, dataArrWidth, dataArrHeight);
		
		// copy result to destination
		/*
		int i, j;
		div_t div_result;
		for(y=0; y<dataArrHeight; y++)
			for(x=0; x<dataArrWidth; x++)
			{	
				// check if coordinates are within bounds and divisible by 2
				div_result = div(x, 2);
				if(div_result.quot<width && y<height)
					if(div_result.rem == 0)
						real[div_result.quot+width*y] = transform[dataArrWidth*y+x];
					else
						imaginary[div_result.quot+width*y] = transform[dataArrWidth*y+x];
					
			}
		*/

		int rix, riy, index;
		const int result_size = dataArrWidth*dataArrHeight+1;

		rix = riy = 0;

		for(index = 1; index < result_size; index++)
		{
			// index is odd - copy real part
			*real[squareWidth*riy+rix]=transform[index];

			// increment index
			index++;

			if(index>=result_size)
				break;

			// index is even - copy imaginary part
			*imaginary[squareWidth*riy+rix]=transform[index];

			// increment coordinates for real and imaginary matrices
			if(rix+1==squareWidth)
			{
				// end of row - go to next row
				rix=0;
				riy++;
				
				if(riy >= squareHeight)
					AfxMessageBox("Unexpected error: riy=>height.");
			}
			else	
				rix++; // stay on this row
		}

		delete transform;

	}
	else // if(transform == 1)
	{
/*		int xn;
		for(xn=1; xn<15; xn++)
			if(pow(2.0, xn)>=width)
				break;
		
		int yn;
		for(yn=1; yn<15; yn++)
			if(pow(2.0, yn)>=height)
				break;
			
		int dataArrWidth, dataArrHeight;
			
		dataArrWidth = (int)pow(2.0, xn) * 2 + 1;
		dataArrHeight = (int)pow(2.0, yn) + 1;
			
		// allocate the bigger image with the new sizes
		float* transform = new float[dataArrWidth*dataArrHeight];
		int x, y;
			
		// fill the data with  zeros
		for(x=0; x<dataArrWidth*dataArrHeight; x++)
			transform[x] = 0;
		
		for(x=0; x<width; x++)
			for(y=0; y<height; y++)
			{
				transform[dataArrWidth*(y+1)+2*x+1]=real[width*y+x];
				transform[dataArrWidth*(y+1)+2*x+2]=imaginary[width*y+x];
			}
			
		// dimensions vector
		unsigned long nn[2];
		nn[1]=(int)pow(2.0, xn);
		nn[2]=(int)pow(2.0, yn);
		
		save_array_float("farrayI.txt", transform, dataArrWidth, dataArrHeight);
		NRfourn((transform+1), nn, 2, -1);
		
		// copy result to destination
		int i, j;
		div_t div_result;
		for(y=0; y<dataArrHeight; y++)
			for(x=0; x<dataArrWidth; x++)
			{	
				// check if coordinates are within bounds and divisible by 2
				div_result = div(x, 2);
				if(div_result.quot<width && y<height)
					if(div_result.rem == 0)
						image[div_result.quot+width*y] = transform[dataArrWidth*y+x];
					
			}

		delete transform;
*/	
	}
	
}
