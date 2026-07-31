// CannyDetector.h: interface for the CCannyDetector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CANNYDETECTOR_H__48FDE3EF_0E34_4BCD_81EB_C5A203739273__INCLUDED_)
#define AFX_CANNYDETECTOR_H__48FDE3EF_0E34_4BCD_81EB_C5A203739273__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCannyDetector  
{
public:
	int EdgeDetect(int* imgData, int img_x, int img_y, int thresL, int thresU, 
		float std, int* edgeData);
	CCannyDetector();
	virtual ~CCannyDetector();

private:
	double Gaussian(double x, double s);
	float ORIENT_SCALE;
	double Hypotenuse(double x, double y);
	int Follow(int i, int j, int low, int cols, int rows, unsigned char * data, unsigned char * magnitude, unsigned char * orientation);
	int ThresholdingTracker(int high, int low, int cols, int rows,  unsigned char* data,  unsigned char*  magnitude, unsigned char* orientation);
	int CannyCore(double s, int cols, int rows, unsigned char* data, unsigned char* derivative_mag, unsigned char* magnitude, unsigned char* orientation);

};

#endif // !defined(AFX_CANNYDETECTOR_H__48FDE3EF_0E34_4BCD_81EB_C5A203739273__INCLUDED_)
