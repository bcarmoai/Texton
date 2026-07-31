// TemplateMatcher.h: interface for the CTemplateMatcher class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_TEMPLATEMATCHER_H__FE27C3ED_BD1C_4C9D_9134_A0A9D9B3BC98__INCLUDED_)
#define AFX_TEMPLATEMATCHER_H__FE27C3ED_BD1C_4C9D_9134_A0A9D9B3BC98__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include <ipl.h>
//#include <cv.h>

#define COV_PARMS 5

class CTemplateMatcher  
{
public:
	float compareError;
	float m_errorThres;
	float m_trainANG;
	BOOL m_templFixed;
	float m_weigthMH;
	float m_trainG;
	float m_trainW;
	int errorCanvas;
	void DrawInitial(int * imgData, int width, int height, void (*progress)(int));
	int templ_xc, templ_yc;
	int iEvaluateCalls;
	char txtFinalParms[200];
	BOOL b_saveCalls;
	float	m_trainLW;
	float	m_trainLH;
	float	m_weigthLW;
	float	m_weigthLH;
	float	m_weightINT;
	float	m_weigthWP;
	float	m_trainWP;
	float		iInitAngle;
	float		m_initG;
	float		m_initLH;
	float		m_initLW;
	float		m_initWP;
	float		m_initW;
	float	m_initAMAG;
	float	m_initGMAG;
	float	m_initLHMAG;
	float	m_initLWMAG;
	float	m_initWMAG;
	float	m_initWPMAG;
	float mean_vector_A[COV_PARMS];
	float SimplexTester(float x[]);
	float PowellTester(float x[]);
	int background;
	float* errorImage;
	IplImage * imgTmpl;
	void match_main(int* imgData, int width, int height, 
		void (*progress)(int), BOOL fixed_position, BOOL save_tofile, 
		float* final_parms = NULL);
	CTemplateMatcher();
	virtual ~CTemplateMatcher();
	float EvaluateTemplate(float angle, float lw, float lh, float w, 
										 float g, float wp, BOOL bEncoded = TRUE);

private:
	float ImageCompare(int* imgData, IplImage* imgTempl, int width, int height);
	BOOL DrawTemplate(IplImage* imgTmpl, int imgWidth, int imgHeight, 
		int x, int y, int angle, int lw, int lh, int w, int g, int wp);

protected:
	float covariance_matrix[COV_PARMS][COV_PARMS];
	void BuildCovarianceMatrix();
	CFile outputFile;
	void EncodeParameters(float& angle, float& lw, float& lh, float& w, float& g, float& wp,
										float& train_wp, float& train_lh, float& train_lw);
	int srcHeight;
	int srcWidth;
	int* srcImage;
	void MinimisePowell(float& angle, float& lh, float& lw, float& w, float& g, float& wp, int& iter);
	void MinimiseSimplex(float& angle, float& lh, float& lw, float& w, float& g, float& wp, int& iter);
	void DecodeParameters(float& angle, float& lw, float& lh, float& w, float& g, float& wp,
										float& train_wp, float& train_lh, float& train_lw);
	void MinimiseSimplexTest();
	IplImage* CreateTmplImage(int width, int height);
};

#endif // !defined(AFX_TEMPLATEMATCHER_H__FE27C3ED_BD1C_4C9D_9134_A0A9D9B3BC98__INCLUDED_)
