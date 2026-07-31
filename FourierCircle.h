// FourierCircle.h: interface for the CFourierCircle class.
//
//////////////////////////////////////////////////////////////////////
//#include <ipl.h>

#if !defined(AFX_FOURIERCIRCLE_H__4F5ED02E_4992_4053_BAD7_8873F4E64B1C__INCLUDED_)
#define AFX_FOURIERCIRCLE_H__4F5ED02E_4992_4053_BAD7_8873F4E64B1C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/*
class CFourierCircle  
{
public:
	void GetDataInverted(int* image);
	void StepInvert();
	void GetDataCombined(int* image);
	void StepCombine();
	void GetDataInvRotatedFFT(int* image);
	void InverseRotatedFFT();
	void GetDataFFTRotated(int* img_fourier);
	void StepFFTRotated();
	void SetFFT(int* image, int width, int height);
	void GetDataRotated(int* image);
	void GetDataInvFFT(int* image);
	void InverseFFT();
	void GetCanny(int* img_canny);
	void StepFFT();
	void GetDataFFT(int* img_fourier);
	void StepRotate(int angle);
	void ResetData();
	void SetCanny(int* img_canny, int img_width, int img_height);
	CFourierCircle();
	virtual ~CFourierCircle();

protected:
	int squareHeight;
	int squareWidth;
	float* imgNRfftReal;
	float* imgNRfftImaginary;
	void NRfournCaller(int* image, float** real, float** imaginary, int width, int height, int forward);
	void NRrlft3(float ***data, float **speq, unsigned long nn1, unsigned long nn2, unsigned long nn3, int isign);
	void NRfourn(float data[], unsigned long nn[], int ndim, int isign);
	IplImage* imgInverted;
	IplImage* imgCombined;
	IplImage* imgRotatedInverse;
	IplImage* imgRotated;
	IplImage* imgInvFFT;
	int imgWidth;
	int imgHeight;
	IplImage* imgRotatedFFT;
	IplImage* imgFFT;
	IplImage* imgCanny;

};
*/
#endif // !defined(AFX_FOURIERCIRCLE_H__4F5ED02E_4992_4053_BAD7_8873F4E64B1C__INCLUDED_)
