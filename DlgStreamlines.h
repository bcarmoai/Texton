#if !defined(AFX_DLGSTREAMLINES_H__36D84DF7_F011_4FEA_AD59_51A6D9D962E5__INCLUDED_)
#define AFX_DLGSTREAMLINES_H__36D84DF7_F011_4FEA_AD59_51A6D9D962E5__INCLUDED_

#include "DataMRI.h"	// Added by ClassView
#include "Data3DMRI.h"
#include "Point3D.h"
#include "PointFloat3D.h"
#include "PointFloatArray.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgStreamlines.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgStreamlines dialog

class CDlgStreamlines : public CDialog
{
// Construction
public:
	int* GetStrmTxtMagn();
	int  GetPointlist(CPointFloatArray* arrayToCopy);
	void NoLoadInitialisePointslist(CData3DMRI* slice3D, int* seeds, int nSeeds);
	void NoLoadInitialiseMask(CData3DMRI *slice3D, float* mask);
	void NoLoadInitialise(CData3DMRI* slice3D);
	void TakeMouseOverPoint(CPoint pointOver);
	void NoLoadShowMR();
	void NoLoadInitialise(CDataMRI* slice);
	void ZoomRestore();
	void ZoomCancel();
	void ZoomOK();
	BOOL bZoom;
	void TakeDownPoint(CPoint clickedPoint);
	void TakeMovePoint(CPoint clickedPoint);
	void TakeUpPoint(CPoint clickedPoint);
	CView* pView;
	CDlgStreamlines(CWnd* pParent = NULL);   // standard constructor
	~CDlgStreamlines();
	CArray<CPointFloat3D, CPointFloat3D> streamlineTest;

// Dialog Data
	//{{AFX_DATA(CDlgStreamlines)
	enum { IDD = IDD_STREAMLS };
	int		m_NoiseSeed;
	int		m_NoiseMean;
	int		m_NoiseVariance;
	int		m_RangeAlpha;
	float	m_LaplWeight;
	int		m_NumberIterations;
	float	m_VelDelta;
	int		m_VortexRange;
	int		m_NoiseBlur;
	float	m_MagMultiplier;
	float	m_IntMultiplier;
	BOOL	m_bAlphaDyn;
	int		m_ZoomSize;
	CString	m_3dImgSizesTxt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStreamlines)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	void AddArrayPoint(float x, float y, float z, CArray<CPointFloat3D, CPointFloat3D> &array);
	void NormaliseInt(int* in_out, int width, int height);
	void NormaliseInt3D(int* in_out, int width, int height, int slices);
	float CalcDeltaS(int direction, int x, int y, float prevx, float prevy, float prevvx, float prevvy, float vel_magn);
	float CalcDeltaS3D(int direction, int x, int y, int z, float prevx, float prevy, float prevz, 
		float prevvx, float prevvy, float prevvz, float vel_magn);
	void ZoomSave();
	BOOL savedDataValid;
	CPoint iZoomUp;
	CPoint iZoomDown;
	CDialog* pDlgZoom;
	void ColourCodeFromMagStream(float vel_mag, float stream, float& red, float& green, float& blue);
	void BlurImage(int* input, int* output, int kernel_size);
	void BlurImage3D(int* input, int* output, int kernel_size);
	int FloatToInt(float fnum);
	void CopyTextureI(int* texture, float* start);
	void CopyTextureI3D(int* texture, float* start);
	void TextureLaplacian3D(float* laplacian);
	void TextureLaplacian(float* laplacian);
	void TextureReset();
	void TextureReset3D();
	void CopyTextureFPI(float * texture, int * start);
	void CopyTextureFP(float* texture, float* start);
	void CopyTextureFP3D(float* texture, float* start);
	CData3DMRI mriData3D;
	CData3DMRI savedData3D;
	CDataMRI mriData;
	CDataMRI savedData;
	float* texture;
	float* texture3D;
	int* noise_img;
	int* noise_img3D;
	int img_height;
	int img_width;
	int TextureIteration(float* start, int alpha, float lapw);
	int TextureIteration3D(float* start, int alpha, float lapw);
	int mi_iterationOption;
	CPointFloatArray* streamlineArray;

	// Generated message map functions
	//{{AFX_MSG(CDlgStreamlines)
	afx_msg void OnOptionStop();
	afx_msg void OnOptionIterations();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancelMode();
	afx_msg void OnStreamlinesGo();
	afx_msg void OnStreamlinesNext();
	afx_msg void OnStrmLoad();
	afx_msg void OnStrmNoise();
	afx_msg void OnShowMR();
	afx_msg void OnShowNse();
	afx_msg void OnShowTxt();
	afx_msg void OnStrmVortex();
	afx_msg void OnStrmResSave();
	afx_msg void OnStrmTxtBlur();
	afx_msg void OnStrmTxtMagn();
	afx_msg void OnStrmZoom10();
	virtual void OnCancel();
	afx_msg void OnStrmTxtColour();
	afx_msg void OnStrmFlipU();
	afx_msg void OnStrmFlipV();
	afx_msg void OnStrmFlipW();
	afx_msg void OnShowU();
	afx_msg void OnShowV();
	afx_msg void OnShowW();
	afx_msg void OnAlphaDyn();
	afx_msg void OnStrmLoad3d();
	afx_msg void OnStrmNoiseGrid();
	afx_msg void OnStrmVtkStreamer();
	afx_msg void OnStrmVtkLinemap();
	afx_msg void OnStrmViewLaplacian();
	afx_msg void OnStrmVtkStreamerSaveLic();
	afx_msg void OnStrmVtkArrows3d();
	afx_msg void OnStrmTxtSeed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	float m_dynAlpha;
	int m_nPointArrays;
	int* m_seedPoints;
	int m_nSeedPoints;
	BOOL m_bPointCollect;
	int* m_iLaplacian;
	int* iStrmMask;
	BOOL mode3D;
	int img_slices;
	void EqualiseInt(int* img_data, int img_width, int img_height);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTREAMLINES_H__36D84DF7_F011_4FEA_AD59_51A6D9D962E5__INCLUDED_)
