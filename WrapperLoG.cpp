// WrapperLoG.cpp: implementation of the CWrapperLoG class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "WrapperLoG.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************
** adapted from code written by          **
** Chris Wyman (12/4/2000)               **
******************************************/


CWrapperLoG::CWrapperLoG()
{

}

CWrapperLoG::~CWrapperLoG()
{

}

/* 
** This applies a Laplacian to an *already smoothed* 
** (by a gaussian) image
*/
texture* CWrapperLoG::MarrHildreth1 ( texture *in )
{
  int i,j;
  texture *out=0;

  if (!in) AfxMessageBox("No input provided to Marr-Hidreth edge detector!");
  out = NewTexture( FLOAT, in->img_width, in->img_height );

  for (j=1;j<in->img_height-1;j++)
    for (i=1;i<in->img_width-1;i++)
      {
	fRedPart(out,i,j)=-4*RedPart(in,i,j)+RedPart(in,i-1,j)+
	  RedPart(in,i+1,j)+RedPart(in,i,j-1)+RedPart(in,i,j+1);
	fGreenPart(out,i,j)=-4*GreenPart(in,i,j)+GreenPart(in,i-1,j)+
	  GreenPart(in,i+1,j)+GreenPart(in,i,j-1)+GreenPart(in,i,j+1);
	fBluePart(out,i,j)=-4*BluePart(in,i,j)+BluePart(in,i-1,j)+
	  BluePart(in,i+1,j)+BluePart(in,i,j-1)+BluePart(in,i,j+1);
      }

  /* handle all the edge cases...  here, just repeat edge pixels
  ** outside of the boundary
  */
  for (i=1;i<in->img_width-1;i++)
    {
      int h=in->img_height-1;
      fRedPart(out,i,0)=-4*RedPart(in,i,0)+RedPart(in,i-1,0)+
	RedPart(in,i+1,0)+RedPart(in,i,0)+RedPart(in,i,1);
      fGreenPart(out,i,0)=-4*GreenPart(in,i,0)+GreenPart(in,i-1,0)+
	GreenPart(in,i+1,0)+GreenPart(in,i,0)+GreenPart(in,i,1);
      fBluePart(out,i,0)=-4*BluePart(in,i,0)+BluePart(in,i-1,0)+
	BluePart(in,i+1,0)+BluePart(in,i,0)+BluePart(in,i,1);
      fRedPart(out,i,h)=-4*RedPart(in,i,h)+RedPart(in,i-1,h)+
	RedPart(in,i+1,h)+RedPart(in,i,h)+RedPart(in,i,h-1);
      fGreenPart(out,i,h)=-4*GreenPart(in,i,h)+GreenPart(in,i-1,h)+
	GreenPart(in,i+1,h)+GreenPart(in,i,h)+GreenPart(in,i,h-1);
      fBluePart(out,i,h)=-4*BluePart(in,i,h)+BluePart(in,i-1,h)+
	BluePart(in,i+1,h)+BluePart(in,i,h)+BluePart(in,i,h-1);
    }
  for (i=1;i<in->img_height-1;i++)
    {
      int w=in->img_width-1;
      fRedPart(out,0,i)=-4*RedPart(in,0,i)+RedPart(in,0,i-1)+
	RedPart(in,0,i+1)+RedPart(in,0,1)+RedPart(in,1,i);
      fGreenPart(out,0,i)=-4*GreenPart(in,0,i)+GreenPart(in,0,i-1)+
	GreenPart(in,0,i+1)+GreenPart(in,0,i)+GreenPart(in,1,i);
      fBluePart(out,0,i)=-4*BluePart(in,0,i)+BluePart(in,0,i-1)+
	BluePart(in,0,i+1)+BluePart(in,0,i)+BluePart(in,1,i);
      fRedPart(out,w,i)=-4*RedPart(in,w,i)+RedPart(in,w,i-1)+
	RedPart(in,w,i+1)+RedPart(in,w,i)+RedPart(in,w-1,i);
      fGreenPart(out,w,i)=-4*GreenPart(in,w,i)+GreenPart(in,w,i-1)+
	GreenPart(in,w,i+1)+GreenPart(in,w,i)+GreenPart(in,w-1,i);
      fBluePart(out,w,i)=-4*BluePart(in,w,i)+BluePart(in,w,i-1)+
	BluePart(in,w,i+1)+BluePart(in,w,i)+BluePart(in,w-1,i);
    }
  
  /* handle the corner cases */
  {
    int h=in->img_height-1;
    int w=in->img_width-1;
    fRedPart(out,0,0)=-2*RedPart(in,0,0)+RedPart(in,1,0)+RedPart(in,0,1);
    fGreenPart(out,0,0)=-2*GreenPart(in,0,0)+GreenPart(in,1,0)+GreenPart(in,0,1);
    fBluePart(out,0,0)=-2*BluePart(in,0,0)+BluePart(in,1,0)+BluePart(in,0,1); 
    fRedPart(out,0,h)=-2*RedPart(in,0,h)+RedPart(in,1,h)+RedPart(in,0,h-1);
    fGreenPart(out,0,h)=-2*GreenPart(in,0,h)+GreenPart(in,1,h)+GreenPart(in,0,h-1);
    fBluePart(out,0,h)=-2*BluePart(in,0,h)+BluePart(in,1,h)+BluePart(in,0,h-1);
    fRedPart(out,w,0)=-2*RedPart(in,w,0)+RedPart(in,w-1,0)+RedPart(in,w,1);
    fGreenPart(out,w,0)=-2*GreenPart(in,w,0)+GreenPart(in,w-1,0)+GreenPart(in,w,1);
    fBluePart(out,w,0)=-2*BluePart(in,w,0)+BluePart(in,w-1,0)+BluePart(in,w,1);
    fRedPart(out,w,h)=-2*RedPart(in,w,h)+RedPart(in,w-1,h)+RedPart(in,w,h-1);
    fGreenPart(out,w,h)=-2*GreenPart(in,w,h)+GreenPart(in,w-1,h)+GreenPart(in,w,h-1);
    fBluePart(out,w,h)=-2*BluePart(in,w,h)+BluePart(in,w-1,h)+BluePart(in,w,h-1);
  }

  return out;
}

/* finds a minimum and maximum values of an image */
void CWrapperLoG::ImageMaxMin( texture *in, float *max, float *min )
{
  int i,j;
  float m1, m2;

  if (!in) AfxMessageBox("No image passed to ImageMaxValue()!");
  if (in->type!=GL_FLOAT) { *max=*min=0; return; }

  m1=m2=(*(in->fImage)+*(in->fImage+1)+*(in->fImage+2));

  for (i=0;i<in->img_width;i++)
    for (j=0;j<in->img_height;j++)
      {
	float *ptr = in->fImage+4*(j*in->img_width+i);
	float val = *(ptr)+*(ptr+1)+*(ptr+2);
	m1 = MAX(m1,val);
	m2 = MIN(m2,val);
      }

  *max = m1;
  *min = m2;
}

/* convert a M-H generated image into one that looks reasonable 
** when displayed... 
*/
texture* CWrapperLoG::ConvertImage( texture *in )
{
  float max, min, mult;
  texture *ret;
  int i,j;

  if (!in) AfxMessageBox("No imput passed to ConvertImage()!");
  if (in->type == UBYTE) return in;
  ret = NewTexture( UBYTE, in->img_width, in->img_height );

  ImageMaxMin( in, &max, &min );

  mult = MAX( fabs(max), fabs(min) ) / 3;

  for (i=0;i<in->img_width;i++)
    for (j=0;j<in->img_height;j++)
      {
	float val = 120*(*(in->fImage+4*(j*in->img_width+i))/mult)+128;
	RedPart(ret,i,j)=(char)val;
	GreenPart(ret,i,j)=(char)val;
	BluePart(ret,i,j)=(char)val;
      }
  return ret;
}


int CWrapperLoG::PosNeg( float i1, float i2, char j1, char j2 )
{
  if ((j1-j2)*(j1-j2) < threshold) return 0;
  if (i1>0 && i2<=0) return 1;
  if (i1<0 && i2>0) return 1;
  return 0;
}

/* given 2 magnitudes & 2 corresponding indices, return
** the index of the smalled magnitude
*/
int CWrapperLoG::MinMag( float i1, int j1, float i2, int j2 )
{
  if (fabs(i1)<fabs(i2)) return j1;
  return j2;
}


/* takes the image after the Laplacian is computed, and the original
** image, computes the gradiant magnitue, and then determines if a pixel
** satisfies the requirements to be an edge pixel (gradiant magnitude
** above some threshold, and Laplacian crosses zero between pixels)
*/
texture* CWrapperLoG::LocateEdges( texture *in, texture *orig )
{
  int i, j;
  texture *out;

  if (!in) AfxMessageBox("No input passed to LocateEdges()!!");
  if (in->type!=GL_FLOAT) AfxMessageBox("Expected image with float values in LocateEdges()!");

  out = NewTexture( GL_UNSIGNED_BYTE, in->img_width, in->img_height );

  for(i=0;i<4*in->img_width*in->img_height;i++)
    *(out->texImage+i)=0;

  for (i=0;i<in->img_width-1;i++)
    for (j=0;j<in->img_height-1;j++)
      {
	if (PosNeg(fRedPart(in,i,j),fRedPart(in,i,j+1),
		   RedPart(orig,i,j),RedPart(orig,i,j+1)))
	  RedPart(out,i,MinMag(fRedPart(in,i,j),j,fRedPart(in,i,j+1),j+1))=255;
	if (PosNeg(fRedPart(in,i,j),fRedPart(in,i+1,j),
		   RedPart(orig,i,j),RedPart(orig,i+1,j)))
	  RedPart(out,MinMag(fRedPart(in,i,j),i,fRedPart(in,i+1,j),i+1),j)=255;

	if (PosNeg(fBluePart(in,i,j),fBluePart(in,i,j+1),
		   BluePart(orig,i,j),BluePart(orig,i,j+1)))
	  BluePart(out,i,MinMag(fBluePart(in,i,j),j,fBluePart(in,i,j+1),j+1))=255;
	if (PosNeg(fBluePart(in,i,j),fBluePart(in,i+1,j),
		   BluePart(orig,i,j),BluePart(orig,i+1,j)))
	  BluePart(out,MinMag(fBluePart(in,i,j),i,fBluePart(in,i+1,j),i+1),j)=255;
	
	if (PosNeg(fGreenPart(in,i,j),fGreenPart(in,i,j+1),
		   GreenPart(orig,i,j),GreenPart(orig,i,j+1)))
	  GreenPart(out,i,MinMag(fGreenPart(in,i,j),j,fGreenPart(in,i,j+1),j+1))=255;
	if (PosNeg(fGreenPart(in,i,j),fGreenPart(in,i+1,j),
		   GreenPart(orig,i,j),GreenPart(orig,i+1,j)))
	  GreenPart(out,MinMag(fGreenPart(in,i,j),i,fGreenPart(in,i+1,j),i+1),j)=255;
      }

  return out;
}

/* allocates space for a new texture WxH */
texture* CWrapperLoG::NewTexture( int type, int w, int h )
{
  texture *n;
  n = (texture *)malloc( sizeof( texture ) );
  if (!n) AfxMessageBox("Unable to allocate memory for texture...");
  n->img_width=w;
  n->img_height=h;
  if (type == GL_UNSIGNED_BYTE)
    {
      n->texImage=(char *)calloc(4*w*h, sizeof(char));
	  n->fImage = NULL;
      if (!n->texImage) AfxMessageBox("Unable to allocate memory for texture...");
    }
  if (type == GL_FLOAT)
    {
      n->fImage=(float *)calloc(4*w*h, sizeof(float));
	  n->texImage = NULL;
      if (!n->fImage) AfxMessageBox("Unable to allocate memory for texture...");
    }
  n->type = type;
  n->format = GL_RGBA;
  return n;
}

void CWrapperLoG::InitializeTexture( texture *t )
{
  int i;
  if (!t) return;
  for (i=0;i<4*t->img_width*t->img_height;i++)
    if (t->type==FLOAT)
      *(t->fImage+i)=0;
    else
      *(t->texImage+i)=0;
}

/* frees an allocated texture */
void CWrapperLoG::FreeTexture( texture *in )
{
	if (!in) return;
	if(in->type == GL_UNSIGNED_BYTE)
	{
		if (in->texImage!=NULL)
			free( in->texImage );
	}
	else
		if(in->type == GL_FLOAT)
		{
			if (in->fImage!=NULL)
				free( in->fImage);
		}

	free( in );
}

void CWrapperLoG::DoMarrHil(int * input, int * output, int width, int height, float thres)
{
	// assume all arrays are allocated
	if(input == NULL)
	{
		AfxMessageBox("Input to DoMarrHil() is NULL");
		return;
	}

	if(output == NULL)
	{
		AfxMessageBox("Output argument to DoMarrHil() is NULL");
		return;
	}

	// convert array to texture format
	texture* imgTexture = IntToTexture(input, width, height);
	texture* m_h;
	texture* converted;

	// set threshold parameter
	if(thres == 0)
		threshold = 2; // magic number from Chris Wyman's README
	else
		threshold = thres;

	// call the marr-hildreth operator
	m_h = MarrHildreth1(imgTexture);

	// convert float image for display
	converted = ConvertImage(m_h);

	// convert texture format back to int array
	TextureToInt(converted, output, width, height);
	NormaliseTexture(m_h, output, width, height);
	FreeTexture(imgTexture);
	FreeTexture(m_h);
	//FreeTexture(converted);


}

texture* CWrapperLoG::IntToTexture(int * input, int width, int height)
{

	texture* t;
	t = (texture *)malloc( sizeof ( texture ) );
	
	if (!t) AfxMessageBox( "Unable to allocate memory for texture structure in IntToTexture()" );

	int i, j, c;

	int img_size = 4*height*width;

	if ((t->texImage = (char *)calloc(img_size, sizeof(char))) == NULL)
		AfxMessageBox("Cannot allocate texture image in IntToTexture()");

	t->fImage = NULL;
	
	t->img_height = height;
	t->img_width = width;

	/* read image data */
	c = 0;
	for (i=0; i<t->img_height; i++) {
		for (j=0; j<t->img_width; j++) {
			t->texImage[c++] = input[width*i+j];
			t->texImage[c++] = input[width*i+j];
			t->texImage[c++] = input[width*i+j];
			t->texImage[c++] = 255;
		}
	}
	
	t->type = GL_UNSIGNED_BYTE;
	t->format = GL_RGBA;
	
	return t;
}

void CWrapperLoG::TextureToInt(texture * text, int * output, int width, int height)
{

	if(width != text->img_width || height != text->img_height)
	{
		AfxMessageBox("Image width or height mismatch in TextureToInt()");
		return;
	}

	int x, y;
	int c = 0;
	for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			output[width*y+x] = text->texImage[c];
			c=c+4;
		}

}

void CWrapperLoG::GaussBlur(int * input, int * output, int width, int height, float sigma)
{
	// convert array to texture
	texture *t = IntToTexture(input, width, height);
	texture* tGauss;

	// do the blur
	tGauss = ApplyGaussianFilter(t, sigma);

	// convert texture to array
	output = new int[tGauss->img_height*tGauss->img_width];
	TextureToInt(tGauss, output, width, height);
}

texture* CWrapperLoG::ApplyGaussianFilter( texture *t, int param )
{
	texture *tmp;
	COMPLEX **ptr;
	int x, y;
	if (param!=0)
    {
		FastFourierTransform( t, &ptr, &x, &y );
		GaussianFilter( ptr, x, y, param );
		tmp = InverseFastFourierTransform( ptr, x, y );
    }
	else tmp = t;
	return tmp;
}


/* apply a low pass */
void CWrapperLoG::GaussianFilter( COMPLEX **c, int nx, int ny, double value )
{
	int i,j;
	for (i=0;i<nx;i++)
		for (j=0;j<ny;j++)
		{
			int s, t;
			double mul;
			s = (i<nx/2?i:i-nx);
			t = (j<ny/2?j:j-ny);
			mul = exp( -1*(s*s+t*t)/(2*value*value) );
			c[i][j].real *= mul;
			c[i][j].imag *= mul;
		}
}


/************************************************* 
** Find the magnitude of a complex number       **
*************************************************/
double CWrapperLoG::Mag(COMPLEX *c)
{
	if (!c) return 0;
	return sqrt(c->real*c->real+c->imag*c->imag);
}

/************************************************* 
** Find maximum magnitude in the image          **
*************************************************/
double CWrapperLoG::FindMaxMagnitude(COMPLEX **c, int nx, int ny)
{ 
	double max=0,tmp;
	int i,j;
	if (!c) return 0;
	for (i=0;i<nx;i++)
		for (j=0;j<ny;j++)
		{
			if ((tmp=Mag(&c[i][j])) > max) max=tmp;
		}
		return max;
}

/************************************************* 
** Find maximum magnitude in the image          **
*************************************************/
double CWrapperLoG::FindAverageMagnitude(COMPLEX **c, int nx, int ny)
{ 
	double sum=0;
	int i,j;
	if (!c) return 0;
	for (i=0;i<nx;i++)
		for (j=0;j<ny;j++)
			sum+=Mag(&c[i][j]);
		return sum/(nx*ny);
}


/* quick thing to get FFT working on color images.
** sorry if it's messy 
*/
void CWrapperLoG::FFTColor(texture *img)
{
	int x_pow, y_pow, tmp, i, j, powx, powy;
	int dif_x, dif_y;
	
	/* do some error checks */
	if (!img)
		AfxMessageBox("Attempt to perform FFT on NULL image!");
	
	/* allocate our FFT workspace */
	Powerof2( img->img_width, &x_pow, &tmp );
	if (tmp!=img->img_width) x_pow++;
	Powerof2( img->img_height, &y_pow, &tmp );
	if (tmp!=img->img_height) y_pow++;
	powx=pow(2,x_pow);
	powy=pow(2,y_pow);
	dif_x=(powx-img->img_width)/2;
	dif_y=(powy-img->img_height)/2;
	red = (COMPLEX **)malloc( powx * sizeof( COMPLEX * ) );
	green = (COMPLEX **)malloc( powx * sizeof( COMPLEX * ) );
	blue = (COMPLEX **)malloc( powx * sizeof( COMPLEX * ) );
	for (i = 0; i < powx; i++)
    {
		red[i]=(COMPLEX *)malloc( powy * sizeof( COMPLEX ) );     
		green[i]=(COMPLEX *)malloc( powy * sizeof( COMPLEX ) );   
		blue[i]=(COMPLEX *)malloc( powy * sizeof( COMPLEX ) );   
    }
	for (i=0;i<powx;i++)
    {
		for (j=0;j<powy;j++)
		{
			if ( (i<dif_x) ||
				(j<dif_y) ||
				(i>=dif_x+img->img_width) ||
				(j>=dif_y+img->img_height) )
			{
				red[i][j].real=red[i][j].imag = 0.0;
				green[i][j].real=green[i][j].imag = 0.0;
				blue[i][j].real=blue[i][j].imag = 0.0;
			}
			else
			{
				char *p=TexColor(img,i-dif_x,j-dif_y);
				red[i][j].real=*p/255.0;
				green[i][j].real=*(p+1)/255.0;
				blue[i][j].real=*(p+2)/255.0;
				red[i][j].imag=green[i][j].imag=blue[i][j].imag=0;
			}
		}
    }
	FFT2D(red,powx,powy,1);
	FFT2D(green,powx,powy,1);
	FFT2D(blue,powx,powy,1);
	xx = powx; yy=powy;
}

/* hack to take the color FFT back to an image */
texture* CWrapperLoG::InverseFFTColor( void )
{
	int i, j;
	//  double scale;
	texture *ptr;
	
	/* perform the Inv(FFT) on the image */
	FFT2D(red,xx,yy,-1);
	FFT2D(green,xx,yy,-1);
	FFT2D(blue,xx,yy,-1);
	
	/* allocate the structure that I'm going to return */
	ptr = (texture *)malloc( sizeof(texture) );
	if (!ptr)
		AfxMessageBox("Unable to allocate texture memory in FFT!\n");
	ptr->texImage = (char *)malloc( 4 * xx * yy * sizeof(char));
	if (!ptr->texImage)
		AfxMessageBox("Unable to allocate texture memory in FFT!\n");
	ptr->img_width=xx;
	ptr->img_height=yy;
	
	for (i=0;i<xx;i++)
		for (j=0;j<yy;j++)
		{
			char *p=TexColor(ptr,i,j);
			int temp = Mag(&red[i][j])*255.0;
			*(p) = (temp>255?255:temp);
			temp = Mag(&green[i][j])*255.0;
			*(p+1) = (temp>255?255:temp);
			temp = Mag(&blue[i][j])*255.0;
			*(p+2) = (temp>255?255:temp);
		}
		
		/* return the pointer to the FFT of the intensity */
		return ptr;
}

/************************************************* 
** Take a texture, perform a FFT and return     **
**  transformed texture & complex stuff so the  **
**  FFT can be inverted without losing accuracy **
*************************************************/
void CWrapperLoG::FastFourierTransform(texture *img,COMPLEX ***cplx_rtrn,int *nx,int *ny)
{
	COMPLEX **c;
	int x_pow, y_pow, tmp, i, j, powx, powy;
	int dif_x, dif_y;
	
	/* do some error checks */
	if (!img)
		AfxMessageBox("Attempt to perfomr FFT on NULL image!");
	if (!cplx_rtrn)
		AfxMessageBox("NULL return pointer specified in FFT!");
	
	/* allocate our FFT workspace */
	Powerof2( img->img_width, &x_pow, &tmp );
	if (tmp!=img->img_width) x_pow++;
	Powerof2( img->img_height, &y_pow, &tmp );
	if (tmp!=img->img_height) y_pow++;
	powx=pow(2,x_pow);
	powy=pow(2,y_pow);
	dif_x=(powx-img->img_width)/2;
	dif_y=(powy-img->img_height)/2;
	c = (COMPLEX **)malloc( powx * sizeof( COMPLEX * ) );
	if (!c)
		AfxMessageBox("Unable to allocate memort for FFT!\n");
	for (i = 0; i < powx; i++)
    {
		c[i]=(COMPLEX *)malloc( powy * sizeof( COMPLEX ) );     
		if (!c[i])
			AfxMessageBox("Unable to allocate memort for FFT!\n");
    }
	
	/* move intensity data into correct data structure */
	for (i=0;i<powx;i++)
    {
		for (j=0;j<powy;j++)
		{
			if ( (i<dif_x) ||
				(j<dif_y) ||
				(i>=dif_x+img->img_width) ||
				(j>=dif_y+img->img_height) )
				c[i][j].real=c[i][j].imag = 0.0;
			else
			{
				char *p=TexColor(img,i-dif_x,j-dif_y);
				c[i][j].real=
					((double)(*p)+(double)(*(p+1))+(double)(*(p+2)))/765.0;
				c[i][j].imag=0;
			}
		}
    }
	
	/* perform the FFT on the intensity (greyscale) image */
	tmp = FFT2D(c,powx,powy,1);
	
	if (!tmp)
		AfxMessageBox("Fourier transform failed!");
	
	/* return the pointer to the FFT of the intensity */
	*nx = powx; *ny = powy;
	*cplx_rtrn = c;
}


/************************************************* 
** Go from the fourier domain to the spacial    **
*************************************************/
texture* CWrapperLoG::InverseFastFourierTransform( COMPLEX **c, int nx, int ny )
{
	int tmp, i, j;
	texture *ptr;
	
	/* do some error checks */
	if (!c)
		AfxMessageBox("Attempt to perform Inv(FFT) on NULL input!");
	
	/* perform the Inv(FFT) on the image */
	tmp = FFT2D(c,nx,ny,-1);
	if (!tmp)
		AfxMessageBox("Fourier transform failed!");
	
	/* allocate the structure that I'm going to return */
	ptr = (texture *)malloc( sizeof(texture) );
	if (!ptr)
		AfxMessageBox("Unable to allocate texture memory in FFT!\n");
	ptr->texImage = (char *)malloc( 4 * nx * ny * sizeof(char));
	if (!ptr->texImage)
		AfxMessageBox("Unable to allocate texture memort in FFT!\n");
	ptr->img_width=nx;
	ptr->img_height=ny;
	
	/* put back into a structure I know how to deal with... */
	// scale = FindMaxMagnitude( c, nx, ny );
	
	for (i=0;i<nx;i++)
		for (j=0;j<ny;j++)
		{
			char *p=TexColor(ptr,i,j);
			int temp = Mag(&c[i][j])*255.0;
			if (temp > 255) temp = 255;
			*(p) = *(p+1) = *(p+2) = temp;
		}
		
		/* return the pointer to the FFT of the intensity */
		return ptr;
}


/*************************************************
** The code expects the following function, so  **
**   I figured I'd write it.  Takes in a number **
**   returns m and 2^m, where 2^m is the power  **
**   of two immediately <= the input            **
*************************************************/
int CWrapperLoG::Powerof2(int n, int *m, int *power)
{
	if (n <= 1) return FALSE;
	*m = 0;
	while (n > 1)
    {
		(*m)++;
		n /= 2;
    }
	if (power)
		*power = pow( 2, *m );
	return TRUE;
}

/*****************************************************************
** The rest of this file is code taken from the web page listed **
** both above and below...                                      **
*****************************************************************/

/*  This is the code found on:
http://www.swin.edu.au/astronomy/pbourke/analysis/fft2d/
---------------------------------------------------------------
Perform a 2D FFT inplace given a complex 2D array
The direction dir, 1 for forward, -1 for reverse
The size of the array (nx,ny)
Return false if there are memory problems or
the dimensions are not powers of 2
*/
int CWrapperLoG::FFT2D(COMPLEX **c,int nx,int ny,int dir)
{
	int i,j;
	int m,twopm;
	double *real,*imag;
	
	/* Transform the rows */
	real = (double *)malloc(nx * sizeof(double));
	imag = (double *)malloc(nx * sizeof(double));
	if (real == NULL || imag == NULL)
		return(FALSE);
	if (!Powerof2(nx,&m,&twopm) || twopm != nx)
		return(FALSE);
	for (j=0;j<ny;j++) {
		for (i=0;i<nx;i++) {
			real[i] = c[i][j].real;
			imag[i] = c[i][j].imag;
		}
		FFT(dir,m,real,imag);
		for (i=0;i<nx;i++) {
			c[i][j].real = real[i];
			c[i][j].imag = imag[i];
		}
	}
	free(real);
	free(imag);
	
	/* Transform the columns */
	real = (double *)malloc(ny * sizeof(double));
	imag = (double *)malloc(ny * sizeof(double));
	if (real == NULL || imag == NULL)
		return(FALSE);
	if (!Powerof2(ny,&m,&twopm) || twopm != ny)
		return(FALSE);
	for (i=0;i<nx;i++) {
		for (j=0;j<ny;j++) {
			real[j] = c[i][j].real;
			imag[j] = c[i][j].imag;
		}
		FFT(dir,m,real,imag);
		for (j=0;j<ny;j++) {
			c[i][j].real = real[j];
			c[i][j].imag = imag[j];
		}
	}
	free(real);
	free(imag);
	
	return(TRUE);
}



/*-------------------------------------------------------------------------
This computes an in-place complex-to-complex FFT
x and y are the real and imaginary arrays of 2^m points.
dir =  1 gives forward transform
dir = -1 gives reverse transform

  Formula: forward
  N-1
  ---
  1   \          - j k 2 pi n / N
  X(n) = ---   >   x(k) e                    = forward transform
  N   /                                n=0..N-1
  ---
  k=0
  
	Formula: reverse
	N-1
	---
	\          j k 2 pi n / N
	X(n) =       >   x(k) e                    = forward transform
	/                                n=0..N-1
	---
	k=0
*/
int CWrapperLoG::FFT(int dir,int m,double *x,double *y)
{
	long nn,i,i1,j,k,i2,l,l1,l2;
	double c1,c2,tx,ty,t1,t2,u1,u2,z;
	
	/* Calculate the number of points */
	nn = 1;
	for (i=0;i<m;i++)
		nn *= 2;
	
	/* Do the bit reversal */
	i2 = nn >> 1;
	j = 0;
	for (i=0;i<nn-1;i++) {
		if (i < j) {
			tx = x[i];
			ty = y[i];
			x[i] = x[j];
			y[i] = y[j];
			x[j] = tx;
			y[j] = ty;
		}
		k = i2;
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}
	
	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l=0;l<m;l++) {
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j=0;j<l1;j++) {
			for (i=j;i<nn;i+=l2) {
				i1 = i + l1;
				t1 = u1 * x[i1] - u2 * y[i1];
				t2 = u1 * y[i1] + u2 * x[i1];
				x[i1] = x[i] - t1;
				y[i1] = y[i] - t2;
				x[i] += t1;
				y[i] += t2;
			}
			z =  u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}
	
	/* Scaling for forward transform */
	if (dir == 1) {
		for (i=0;i<nn;i++) {
			x[i] /= (double)nn;
			y[i] /= (double)nn;
		}
	}
	
	return(TRUE);
}

void CWrapperLoG::NormaliseOutput(int * output, int width, int height)
{
	int min;
	int max;
	int x, y;

	min = max = output[0];

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			if(output[width*y+x]>max)
				max = output[width*y+x];

			if(output[width*y+x]<min)
				min = output[width*y+x];
		}

	int range = max - min;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			output[width*y+x]=floor( (output[width*y+x]-min)*(255.0/range)+0.000001 );

}



void CWrapperLoG::NormaliseTexture(texture * text, int * output, int width, int height)
{
	float min;
	float max;
	int x, y;

	min = max = text->fImage[0];

	int c = 0;
		for(y=0; y<height; y++)
	for(x=0; x<width; x++)
		{
			if(text->fImage[c]>max)
				max = text->fImage[c];

			if(text->fImage[c]<min)
				min = text->fImage[c];

			c=c+4;
		}

	float range = max - min;

	c=0;
		for(y=0; y<height; y++)
	for(x=0; x<width; x++)
		{
			output[width*y+x]=floor( (text->fImage[c]-min)*(255.0/range)+0.000001 );
			c=c+4;
		}

}
