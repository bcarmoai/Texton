// CMRStreamlines.cpp: implementation of the CCMRStreamlines class.
//
//////////////////////////////////////////////////////////////////////
//
//   written by Bernardo S. Carmo
//   Imperial College October 2003
//
//   based on Transient Streamlines by Yang et al 2000 IJCI
// 
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "texton.h"
#include "CMRStreamlines.h"
#include <math.h>
#include <float.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Public interface
//////////////////////////////////////////////////////////////////////


int CCMRStreamlines::StreamlinePlot(short *u, short *v, /* SOURCE */
									int *r, int *g, int *b, int *gs /* ALLOCATED BY CALLER */,
									int width, int height,
									float rgb_IntMultiplier /* =1.0f */,
									float rgb_MagMultiplier /* =1.0f */,
									float noise_cover /* =0.5f */, int bAlphaDynamic /* =TRUE */,
									float dynAlpha /* =0.008f */, 
									float laplacian_weigth /* =0.1f */, 
									int iterations /* =5 */)
{
	data_u = u;
	data_v = v;
	img_width = width;
	img_height = height;

	// allocate target image
	if(texture != NULL)
		delete [] texture;	
	texture = new float[img_width*img_height];
	float* start = new float[img_width*img_height];
	
	// random seed image
	OnStrmNoise(noise_cover);
				
	// texture iterations
	int i;
	TextureIteration(noise_img, /*bAlphaDynamic*/FALSE, /*dynAlpha*/10, laplacian_weigth);
	CopyTextureFP(texture, start);
	
	for(i=0; i<=iterations; i++)
	{
		TextureIteration(start, bAlphaDynamic, dynAlpha, laplacian_weigth);
		CopyTextureFP(texture, start);
	}
	
	delete [] start;	
	delete [] noise_img;

	// derive colour and greyscale images
	CopyResult(r, g, b, gs, rgb_IntMultiplier, rgb_MagMultiplier);

	delete [] texture;
	start = noise_img = texture = NULL;
	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCMRStreamlines::CCMRStreamlines()
{
	noise_img = NULL;
	texture = NULL;
}

CCMRStreamlines::~CCMRStreamlines()
{
	if(noise_img!=NULL)
		delete [] noise_img;

}

//////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////

void CCMRStreamlines::CopyTextureFP(float * texture, float * start)
{
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			start[img_width*y+x] = texture[img_width*y+x];
}


int CCMRStreamlines::FloatToInt(float fnum)
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

//////////////////////////////////////////////////////////////////////
// Streamline engine
//////////////////////////////////////////////////////////////////////

int CCMRStreamlines::TextureIteration(float *start, BOOL bDynAlpha, float alpha, float lapw)
{
	int n, x, y, ix, iy;
	float newx, newy, prevx, prevy;
	float vel_magn;
	int term_count;
	double tex_temp;
	float deltas;
	int dynAlpha=(int)alpha;

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			{
				if(bDynAlpha)
					dynAlpha = FloatToInt((float)ceil(5.0f+alpha*sqrt(pow(data_u[y*img_width+x],2)
					+pow(data_v[y*img_width+x],2))));
					// this should map velocity into ~ [5-25] 
				
				tex_temp = 0;
				term_count = 0;
				
				prevx = x+0.5f;
				prevy = y+0.5f;
				
				// positive side
				for(n=1; n<=dynAlpha; n++)
				{
					ix = (int)(floor)(prevx);
					iy = (int)(floor)(prevy);
					vel_magn = (float)sqrt(pow(data_u[iy*img_width+ix],2)
						+pow(data_v[iy*img_width+ix],2));
					deltas = CalcDeltaS(1, ix, iy, prevx, prevy, 
						data_u[iy*img_width+ix], 
						data_v[iy*img_width+ix], vel_magn);
					if(deltas == -1)
						continue;
					newx = prevx + data_u[iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					newy = prevy + data_v[iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					if(newx>=img_width)newx=img_width-1.0f;
					if(newy>=img_height)newy=img_height-1.0f;
					if(newx<0)newx=0;
					if(newy<0)newy=0;
					
					ix = FloatToInt(newx);
					iy = FloatToInt(newy);

					// eliminate streamlines along border
					if(ix>=img_width-1 || iy >= img_height-1 || ix <= 0 || iy <= 0 )
							break;

					tex_temp = tex_temp + start[iy*img_width+ix];
					term_count++;
					
					prevx = newx;
					prevy = newy;
					
				}

				prevx = x+0.5f;
				prevy = y+0.5f;

				// negative side
				for(n=1; n<=dynAlpha; n++)
				{
					ix = (int)(floor)(prevx);
					iy = (int)(floor)(prevy);

					vel_magn = (float)sqrt(pow(data_u[iy*img_width+ix],2)
						+pow(data_v[iy*img_width+ix],2));
					deltas = CalcDeltaS(1, ix, iy, prevx, prevy, 
						(-1.0f)*data_u[iy*img_width+ix], 
						(-1.0f)*data_v[iy*img_width+ix], vel_magn);
					if(deltas == -1)
						break;
					newx = prevx - data_u[iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					newy = prevy - data_v[iy*img_width+ix]*(1.0f/vel_magn)*deltas;
					if(newx>=img_width)newx=img_width-1.0f;
					if(newy>=img_height)newy=img_height-1.0f;
					if(newx<0)newx=0;
					if(newy<0)newy=0;
					
					ix = FloatToInt(newx);
					iy = FloatToInt(newy);
					if(ix>=img_width || iy >= img_height || ix < 0 || iy < 0)
						continue;

					tex_temp = tex_temp + start[iy*img_width+ix];
					term_count++;
					
					prevx = newx;
					prevy = newy;
					
				}
				// zero tau part
				tex_temp = tex_temp + start[y*img_width+x];
				term_count++;
				
				// normalisation
				texture[y*img_width+x] = (float)tex_temp/(2.0f*(float)dynAlpha+1.0f);
				
			}

	// take laplacian
	float* text_lap = new float[img_width*img_height];
	TextureLaplacian(text_lap);

	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			{
				if(text_lap[y*img_width+x]>0.0f)
				texture[y*img_width+x] = 
					texture[y*img_width+x] + 
					lapw*text_lap[y*img_width+x];
			}
		
	delete [] text_lap;

	return 0;
}

float CCMRStreamlines::CalcDeltaS(int direction, int x, int y, float prevx, float prevy, 
				 float prevvx, float prevvy, float vel_magn)
{
	double svals[4];

	svals[0] = ((y+1)-prevy)*(vel_magn/prevvy);
	svals[1] = (y-prevy)*(vel_magn/prevvy);
	svals[2] = ((x+1)-prevx)*(vel_magn/prevvx);
	svals[3] = (x-prevx)*(vel_magn/prevvx);

	float min = FLT_MAX;
	BOOL min_set = FALSE;
	for(int i=0; i<4; i++)
		if(svals[i]<min && svals[i]>0 && _finite(svals[i]))
		{
			min=(float)svals[i];
			min_set = TRUE;

		}

	if(min_set == FALSE)
		return -1;
	else
//		if(min > 0.7071066 && min < 0.7071068)
			return min+0.01f;
//		else
//			return min;

}

void CCMRStreamlines::TextureLaplacian(float * laplacian)
{
		
	const int lsize = 5;
	float coeffs[lsize*lsize]={-1.0f,-3.0f,-4.0f,-3.0f,-1.0f,-3.0f,0.0f,6.0f,
		0.0f,-3.0f,-4.0f,6.0f,20.0f,6.0f,-4.0f,-3.0f,0.0f,6.0f,0.0f,-3.0f,-1.0f,
		-3.0f,-4.0f,-3.0f,-1.0f};
	
	/*
	const int lsize = 3;
	float coeffs[lsize*lsize]={-1.0f,-1.0f,-1.0f,-1.0f,8.0f,-1.0f,-1.0f,-1.0f,-1.0f};
	*/

	// convolve with image
	int x, y;
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			laplacian[y*img_width+x]=0.0;

	int cx, cy;
	for(x=2; x<img_width-2; x++)
		for(y=2; y<img_height-2; y++)
		{
			for(cx=0; cx<lsize; cx++)
				for(cy=0; cy<lsize; cy++)
				{
					laplacian[y*img_width+x]
						= laplacian[y*img_width+x]
						+ texture[(y-2+cy)*img_width+(x-2+cx)]
						* coeffs[cy*lsize+cx];
				}					
		}
}

/* random generator for seeds image */

#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   
#define UPPER_MASK 0x80000000 
#define LOWER_MASK 0x7fffffff 

#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

void CCMRStreamlines::SetGenRand(unsigned long seed)
{
  mt[0]= seed & 0xffffffff;
  for (mti=1; mti<N; mti++)
    mt[mti] = (69069 * mt[mti-1]) & 0xffffffff;
}

double CCMRStreamlines::GenRand()
{
  unsigned long y;
  static unsigned long mag01[2]={0x0, MATRIX_A};

  if (mti >= N) { 
    int kk;

    if (mti == N+1)   
      SetGenRand(4357); 

    for (kk=0;kk<N-M;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    for (;kk<N-1;kk++) {
      y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
      mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
    }
    y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
    mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];
    
    mti = 0;
  }
  
  y = mt[mti++];
  y ^= TEMPERING_SHIFT_U(y);
  y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
  y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
  y ^= TEMPERING_SHIFT_L(y);
  
  return ( (double)y / (unsigned long)0xffffffff ); 
}

double CCMRStreamlines::GenRand(double lo, double hi)
{
  double res;
  res = lo + (hi - lo)*GenRand();
  return res;
}

void CCMRStreamlines::OnStrmNoise(float noise_mean) 
{	
	const int noise_seed = 1;

	if(img_width == 0 || img_height == 0)
		return;

	mti = N+1; 

	int x, y;
	if(noise_img!=NULL)
		delete [] noise_img;
	noise_img = new float[img_width*img_height];
	for(x=0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			noise_img[y*img_width+x] = 0;
			
	float a = noise_mean*100;
	int i, j, amt;
	a = (a < 0) ? 0:a;
	a = (a > 100) ? 100:a;
	amt = (int)FloatToInt(img_height*img_width*a/100.0f);
	SetGenRand(noise_seed);
	for (int k = 0; k < amt; k++) 
	{
		j = (int)FloatToInt((float)GenRand(0, img_width-1));
		i = (int)FloatToInt((float)GenRand(0, img_height-1));
		noise_img[i*img_width+j] 
			= (float)(255*FloatToInt((float)GenRand()));			
	}


}

void CCMRStreamlines::CopyResult(int *r, int *g, int *b, int *gs, 
								 float rgb_IntMultiplier, float rgb_MagMultiplier)
{
	float fred, fgreen, fblue;
	int* mag_img = new int[img_width*img_height];
	int* norm_text = new int[img_width*img_height];
	
	int x, y;
	for(x = 0; x<img_width; x++)
		for(y=0; y<img_height; y++)
				mag_img[y*img_width+x] = 
				FloatToInt((float)sqrt(
				 pow(data_u[y*img_width+x],2)
				+pow(data_v[y*img_width+x],2) ));

	for(x = 0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			norm_text[y*img_width+x] = 
			FloatToInt(texture[y*img_width+x]);

	NormaliseInt(norm_text, img_width, img_height);
	NormaliseInt(mag_img, img_width, img_height);

	for(x = 0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			mag_img[y*img_width+x] = 
			FloatToInt(mag_img[y*img_width+x]*rgb_MagMultiplier);

	for(x = 0; x<img_width; x++)
		for(y=0; y<img_height; y++)
			norm_text[y*img_width+x] = 
			FloatToInt(norm_text[y*img_width+x]*rgb_IntMultiplier);

	for(x = 0; x<img_width; x++)
		for(y=0; y<img_height; y++)
		{	
			ColourCodeFromMagStream((float)mag_img[y*img_width+x], 
				(float)norm_text[y*img_width+x], 
				fred, fgreen, fblue);
			
			r[y*img_width+x] = FloatToInt(fred);
			g[y*img_width+x] = FloatToInt(fgreen);
			b[y*img_width+x] = FloatToInt(fblue);
			gs[y*img_width+x] = mag_img[y*img_width+x]*norm_text[y*img_width+x];
		}

	NormaliseInt(gs, img_width, img_height);

	delete [] mag_img;
	delete [] norm_text;

}


void CCMRStreamlines::NormaliseInt(int * in_out, int width, int height)
{
	int min;
	int max;
	int x, y;

	min = max = in_out[0];

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
		{
			if(in_out[width*y+x]>max)
				max = in_out[width*y+x];

			if(in_out[width*y+x]<min)
				min = in_out[width*y+x];
		}

	int range = max - min;
	float m, b;
	m=255.0f/range;
	b=255.0f-m*(float)max;

	for(x=0; x<width; x++)
		for(y=0; y<height; y++)
			in_out[width*y+x]=FloatToInt( in_out[width*y+x]*m+b+0.00001f );

}

void CCMRStreamlines::ColourCodeFromMagStream(float vel_mag, float stream, float & red, 
										  float & green, float & blue)
{

	float intensity = (vel_mag/255.0f)*(stream/255.0f);

	if(intensity > 1)
		intensity = 1;

	if(vel_mag < 256.0f && vel_mag > 128.0f)
	{
		red = ((255.0f/(255.0f-128.0f))*vel_mag-255.0f) * intensity;
		green = ((-255/(255.0f-128.0f))*vel_mag+510.0f) * intensity;
		blue = 0.0f;
	}
	else
		if(vel_mag <= 128.0f && vel_mag >= 0.0f)
		{
			red = 0.0f;
			green = (255.0f/128.0f)*vel_mag * intensity;
			blue = ((-255.0f/128.0f)*vel_mag+255.0f) * intensity;
		}
		else
		{
			red = 255.0f;
			green = 255.0f;
			blue = 255.0f;
		}

}