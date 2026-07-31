#if !defined(AFX_DLGRESTORATION_H__E1FE13B3_71BA_4337_A8EF_BED7DCB05662__INCLUDED_)
#define AFX_DLGRESTORATION_H__E1FE13B3_71BA_4337_A8EF_BED7DCB05662__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgRestoration.h : header file
//

#include "DlgStreamlines.h"
// #include "DataMRI.h"	// Added by ClassView
#include "Data3DMRI.h"	// Added by ClassView
#include "Point3D.h"

#define LDOUBLE long double

/////////////////////////////////////////////////////////////////////////////
// CDlgRestoration dialog

class CDlgRestoration : public CDialog
{
// Construction
public:
	void RoiWhole();
	BOOL bZoom;
	void TakeMouseOverPoint(CPoint clickedPoint);
	void TakeDownPoint(CPoint clickedPoint);
	void TakeMovePoint(CPoint clickedPoint);
	void TakeUpPoint(CPoint clickedPoint);
	void RoiCancel();
	void RoiOK();
	CString sThreadString;
	BOOL bThreadString;
	BOOL bThreadRun;
	void StreamlinesClosed();
	CView* pView;
	CDlgRestoration(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRestoration)
	enum { IDD = IDD_RESTORATION };
	int		m_3DVortexRange;
	int		m_RestoredSliceNo;
	int		m_3DNoiseS;
	float	m_3DNoiseM;
	float	m_3DNoiseV;
	int		m_3dRestorVortexSlices;
	float	m_3DRestorLambda;
	float	m_3DRestorMinLambdaDiff;
	CString	m_3DRestorRoiText;
	int		m_3DRestorSliceEnd;
	BOOL	m_Restor3dLambdaFixed;
	float	m_3DRestorSigma;
	float	m_3DRestorDeltaT;
	int		m_3DRestorIterations;
	CString	m_Restor3dImgSizesTxt;
	float	m_fZoomScale;
	BOOL	m_ZeroSkip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRestoration)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:	
	LDOUBLE m_3DRestorMinEnergyDiff;
	float	m_3DRestorLambdaFinal;
	void Restor3dVis(CData3DMRI& source);
	void Restor3dInterface(LDOUBLE lambda, LDOUBLE ctve, LDOUBLE utve, LDOUBLE ctutve, LDOUBLE rms);
	void NormaliseInt(int * in_out, int width, int height);
	void Restor3dGetNeighs(CPoint3D* points, int* neigh_count, int img_width, int img_height,
					   int img_slices, int curx, int cury, int curz);
	double Restor3dIterationRMS(short* u, short* v, 
		short* w, short* ori_u,
		short* ori_v, short* ori_w);
	double Restor3dIterationRMS(LDOUBLE* u, LDOUBLE* v, 
		LDOUBLE* w, short* ori_u,
		short* ori_v, short* ori_w);
	void Restor3dIterationE_Map(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* e_map);
	void Restor3dIterationWtMap(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* uwt_map, 
		LDOUBLE* vwt_map, LDOUBLE* wwt_map, LDOUBLE* e_map);	
	void Restor3dIterationBigPi(LDOUBLE* ubigpi, LDOUBLE* vbigpi, LDOUBLE* wbigpi, LDOUBLE* unoisy, 
		LDOUBLE* vnoisy, LDOUBLE* wnoisy, LDOUBLE* uwt_map, LDOUBLE* vwt_map, 
		LDOUBLE* wwt_map, LDOUBLE lambda, LDOUBLE* u, LDOUBLE* v, LDOUBLE* w);	
	LDOUBLE Restor3dIterationHU(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* noisyu, LDOUBLE* noisyv, 
		LDOUBLE* noisyw, LDOUBLE ht_rterm);
	LDOUBLE Restor3dIterationUTVE(LDOUBLE* u, LDOUBLE* v, LDOUBLE* w, LDOUBLE* e_map);
	void Restor3dIterationProjectVector(LDOUBLE & up, LDOUBLE & vp, 
													 LDOUBLE & wp, LDOUBLE u, 
													 LDOUBLE v, LDOUBLE w);
	CData3DMRI mriData3DRestored;
	void Noise3D(float* img_data, int img_width, int img_height, int img_slices, 
		int seed, float mean, float variance);
	CData3DMRI mriData3DNoisy;
	double Restor2dIterationUTVE(float* u, float* v, float* w, float* e_map);
	int iRestor2d_xtop;
	int iRestor2d_ytop;
	int iRestor2d_xbottom;
	int iRestor2d_ybottom;
	int iRestor2d_prevxtop;
	int iRestor2d_prevytop;
	int iRestor2d_prevxbottom;
	int iRestor2d_prevybottom;
	CPoint iRoiUp;
	CPoint iRoiDown;
	CDialog* pDlgRoi;
	void Restor2dIterationProjectVector(float& up, float& vp, float& wp, float u, float v, float w);
	void Restor2dGetNeighs(CPoint* points, int* neigh_count, int img_width, int img_height,
		int curx, int cury);
	void Restor2dIterationE_Map(float* u, float* v, float* w, float* e_map);
	void Restor2dIterationWtMap(float* u, float* v, float* w, float* uwt_map, 
		float* vwt_map, float* wwt_map, float* e_map);	
	void Restor2dIterationBigPi(float* ubigpi, float* vbigpi, float* wbigpi, float* unoisy, 
		float* vnoisy, float* wnoisy, float* uwt_map, float* vwt_map, 
		float* wwt_map, float lambda, float* u, float* v, float* w);	
	float Restor2dIterationHU(float* u, float* v, float* w, float* noisyu, float* noisyv, 
		float* noisyw, float ht_rterm);
	CDataMRI mriData2DRestored;
	CDataMRI mriData2DNoisy;
	// void BlurImage(float* input, float* output, int kernel_size);
	int FloatToInt(float fnum);
	void Noise2D(float* img_data, int img_width, int img_height,
				 int seed, float mean, float variance, int noiseBlur);
	CData3DMRI mriData3D;
	// CDataMRI mriData2D;
	CDlgStreamlines* pStrm;

	// Generated message map functions
	//{{AFX_MSG(CDlgRestoration)
	virtual void OnCancel();
	afx_msg void OnRestor3dvortex();
	afx_msg void OnRestor3dvisData();
	afx_msg void OnRestor3dNoiseGo();
	afx_msg void OnRestor3dvisNoisy();
	afx_msg void OnRestor3dRoi();
	afx_msg void OnRestor3dgo();
	afx_msg void OnRestor3dvisRestored();
	afx_msg void OnRestor3dload();
	afx_msg void OnRestor3dloadFrame();
	afx_msg void OnRestorFindVortex();
	afx_msg void OnRestor3dvisData2();
	afx_msg void OnRestor3dvisNoisydata();
	afx_msg void OnRestor3dHeartLoad();
	afx_msg void OnButton1();
	afx_msg void OnRestorCcfdRoiPreset();
	afx_msg void OnRestorLoad7D();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESTORATION_H__E1FE13B3_71BA_4337_A8EF_BED7DCB05662__INCLUDED_)
