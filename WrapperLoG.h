// WrapperLoG.h: interface for the CWrapperLoG class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WRAPPERLOG_H__3EA0A31B_A10F_484B_B96B_FE0F1DE5A300__INCLUDED_)
#define AFX_WRAPPERLOG_H__3EA0A31B_A10F_484B_B96B_FE0F1DE5A300__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/******************************************
** adapted from code written by          **
** Chris Wyman (12/4/2000)               **
******************************************/

/* macros to get texture color */
#define TexColor(t,x,y) ((t)->texImage+4*((y)*(t)->img_width+(x)))
#define GetColor TexColor
#define RedPart(t,x,y) (*(TexColor(t,x,y)))
#define GreenPart(t,x,y) (*((TexColor(t,x,y))+1))
#define BluePart(t,x,y) (*((TexColor(t,x,y))+2))
#define AlphaPart(t,x,y) (*((TexColor(t,x,y))+3))
#define fTexColor(t,x,y) ((t)->fImage+4*((y)*(t)->img_width+(x)))
#define fRedPart(t,x,y) (*(fTexColor(t,x,y)))
#define fGreenPart(t,x,y) (*((fTexColor(t,x,y))+1))
#define fBluePart(t,x,y) (*((fTexColor(t,x,y))+2))
#define fAlphaPart(t,x,y) (*((fTexColor(t,x,y))+3))

/* accepted valuse for texture.type */
#define GL_UNSIGNED_BYTE                    0x1401
#define GL_FLOAT                            0x1406
#ifndef UBYTE
#define UBYTE GL_UNSIGNED_BYTE
#endif
#ifndef FLOAT
#define FLOAT GL_FLOAT
#endif

/* accepted values for texture.format */
#define GL_RGBA                             0x1908
#ifndef RGBA
#define RGBA GL_RGBA
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif
#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif
#ifndef SQR
#define SQR(x)   ((x)*(x))
#endif

/* 
** 'texture' structure, what images are read into
** or written from
*/
typedef struct {
  int img_width, img_height;
  char *texImage;
  float *fImage;
  int type;
  int format;
} texture;

/* blargh! */
typedef struct {
  double real, imag;
} COMPLEX;

class CWrapperLoG  
{
public:
	void GaussBlur(int * input, int * output, int width, int height, float sigma);
	void DoMarrHil(int* input, int* output, int width, int height, float thres);
	CWrapperLoG();
	virtual ~CWrapperLoG();

protected:
	void NormaliseTexture(texture* text, int* output, int width, int height);
	void NormaliseOutput(int* output, int width, int height);
	// interface functions
	void TextureToInt(texture* text, int* output, int width, int height);
	texture* IntToTexture(int* input, int width, int height);
	texture *NewTexture( int type, int w, int h );
	void FreeTexture( texture *t );
	void InitializeTexture( texture *t );
	texture* ConvertImage( texture *in );
	// marr-hildreth functions
	texture *MarrHildreth1 ( texture *in );
	texture *LocateEdges( texture *in, texture *orig );
	int MinMag( float i1, int j1, float i2, int j2 );
	int PosNeg( float i1, float i2, char j1, char j2 );
	void ImageMaxMin( texture *in, float *max, float *min );
	float threshold;
	// gaussian and FFT functions
	COMPLEX **red, **green, **blue;
	int xx, yy;
	int FFT2D(COMPLEX **c,int nx,int ny,int dir);
	int FFT(int dir,int m,double *x,double *y);
	int Powerof2(int n, int *m, int *power);
	texture* ApplyGaussianFilter( texture *t, int param );
	void GaussianFilter( COMPLEX **c, int nx, int ny, double value );
	double Mag(COMPLEX *c);
	double FindMaxMagnitude(COMPLEX **c, int nx, int ny);
	double FindAverageMagnitude(COMPLEX **c, int nx, int ny);
	void FFTColor(texture *img);
	texture* InverseFFTColor( void );
	void FastFourierTransform(texture *img,COMPLEX ***cplx_rtrn,int *nx,int *ny);
	texture* InverseFastFourierTransform( COMPLEX **c, int nx, int ny );

};




#endif // !defined(AFX_WRAPPERLOG_H__3EA0A31B_A10F_484B_B96B_FE0F1DE5A300__INCLUDED_)
