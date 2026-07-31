// CMRStreamlines.h: interface for the CCMRStreamlines class.
//
//////////////////////////////////////////////////////////////////////
//
//   written by Bernardo S. Carmo
//   Imperial College October 2003
//
//   based on Transient Streamlines by Yang et al 2000 IJCI
// 
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_CMRSTREAMLINES_H__F7520F2E_F7F0_43D7_B082_033E236C9AB4__INCLUDED_)
#define AFX_CMRSTREAMLINES_H__F7520F2E_F7F0_43D7_B082_033E236C9AB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCMRStreamlines  
{
public:
	int StreamlinePlot(
		short* u, short* v, /* SOURCE vector components */
		/* in implementation, array is referred to as u[width*y+x] */
		int* r, int* g, int* b, /* target red, green, blue components ALLOCATED BY CALLER */
		int* gs, /* target greyscale components for caller to use own RGB mapping ALLOCATED BY CALLER */
		int width = 0, int height = 0,
		float rgb_IntMultiplier = 1.0f /* intensity multiplier to create RGB image */,
		/* set >1 for brighter lines */
		float rgb_MagMultiplier = 1.0f /* colour range multiplier to create RGB image */,
		/* set >1 to bias lines colour towards red, <1 towards blue*/
		float noise_cover=0.5, /* random seeds will be placed in 50% of the image */
		int bAlphaDynamic = TRUE, /* TRUE=longer lines for higher vector mags, FALSE=fixed line length */
		float dynAlpha = 0.008f, /* for dynamic alpha, must turn source vel components into range [0-20] */
		/* default = 0.008 good for typ vel range [0-2500] -> [0-20]; if bAlphaDynamic = FALSE, set this to 10.0f*/
		float laplacian_weigth = 0.1f, /* how much to multiply streamlines by edge detector for sharper image */
		int iterations = 5 /* more iterations = smoother output but longer processing */
		);
	CCMRStreamlines();
	virtual ~CCMRStreamlines();

protected:
	void ColourCodeFromMagStream(float vel_mag, float stream, float & red, float & green, float & blue);
	void NormaliseInt(int * in_out, int width, int height);
	void CopyResult(int* r, int* g, int* b, int* gs, float rgb_IntMultiplier, float rgb_MagMultiplier);
	void CopyTextureFP(float * texture, float * start);
	float* noise_img;
	void OnStrmNoise(float noise_mean);
	short* data_v;
	short* data_u;
	void TextureLaplacian(float * laplacian);
	float CalcDeltaS(int direction, int x, int y, float prevx, float prevy, 
				 float prevvx, float prevvy, float vel_magn);
	int FloatToInt(float fnum);
	int TextureIteration(float* start, BOOL bDynAlpha, float alpha, float lapw);
	int img_height;
	int img_width;
	float* texture;
	void SetGenRand(unsigned long seed);
	double GenRand();
	double GenRand(double lo, double hi);
	unsigned long mt[624]; // for noise generator
	int mti; // for noise generator



};

#endif // !defined(AFX_CMRSTREAMLINES_H__F7520F2E_F7F0_43D7_B082_033E236C9AB4__INCLUDED_)
