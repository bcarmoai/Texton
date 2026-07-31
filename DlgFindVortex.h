#if !defined(AFX_DLGFINDVORTEX_H__DF4CCE03_0C25_4DE3_AF0D_13FDF0EC5DE7__INCLUDED_)
#define AFX_DLGFINDVORTEX_H__DF4CCE03_0C25_4DE3_AF0D_13FDF0EC5DE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFindVortex.h : header file
//

#include "Data3DMRI.h"	
#include "DlgVolView.h"
#include "DlgStreamlines.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgFindVortex dialog

class CDlgFindVortex : public CDialog
{
// Construction
public:
	float* SeedMask();
	float* GetVelocityMagn();
	float* GetVorticityMagn();
	int* m_RegionCriticalPoints;
	void VtkSaveVorticityArrows(BOOL b3D, BOOL bHCFD, BOOL bs, int zerob_s, BOOL bx, int ix, BOOL by, int iy);
	CString VtkSaveArrows(BOOL b3D, BOOL bHCFD, BOOL bs, int zerob_s, BOOL bx, int ix, BOOL by, int iy, int i_fname = 0);
	void NoLoadInitialise(CData3DMRI *slice3D);
	void TakeMovePoint(CPoint point);
	BOOL bTakeMouse;
	CData3DMRI mriData3D;
	CView* pView;
	CDlgFindVortex(CWnd* pParent = NULL);   // standard constructor
	float* m_lmsDistances;
	float LmsComputeApbDistance3D(float** matrix_a, float x0, float y0, float z0);

// Dialog Data
	//{{AFX_DATA(CDlgFindVortex)
	enum { IDD = IDD_FINDVORTEX };
	CString	m_ImgSizesTxt;
	float	m_selMagnThres;
	CString	m_eigenPosTxt;
	CString	m_eigen1Txt;
	CString	m_eigen2Txt;
	CString	m_eigen3Txt;
	CString	m_dudxTxt;
	CString	m_dudyTxt;
	CString	m_dudzTxt;
	CString	m_dvdxTxt;
	CString	m_dvdyTxt;
	CString	m_dvdzTxt;
	CString	m_dwdxTxt;
	CString	m_dwdyTxt;
	CString	m_dwdzTxt;
	float	m_vortexRange;
	int		m_vortexSlices;
	CString	m_uFlippedTxt;
	CString	m_vFlippedTxt;
	CString	m_wFlippedTxt;
	int		m_windingCircleRad;
	double	m_windingTol;
	CString	m_vorticityX;
	CString	m_vorticityY;
	CString	m_vorticityZ;
	CString	m_dudxLM;
	CString	m_dudyLM;
	CString	m_dudzLM;
	CString	m_dvdxLM;
	CString	m_dvdyLM;
	CString	m_dvdzLM;
	CString	m_dwdxLM;
	CString	m_dwdyLM;
	CString	m_dwdzLM;
	CString	m_eig1LM;
	CString	m_eig2LM;
	CString	m_eig3LM;
	CString	m_lambdaAtPosition;
	BOOL	m_strmVorticity;
	float	m_strmVorticityThres;
	BOOL	m_strmMagn;
	float	m_strmMagnThres;
	BOOL	m_strmEigIm;
	float	m_strmEigImThres;
	BOOL	m_strmPos;
	int		m_strmPosX;
	int		m_strmPosY;
	int		m_strmPosZ;
	BOOL	m_strmMagnitudeValid;
	BOOL	m_strmImagValid;
	BOOL	m_strmVorticityValid;
	BOOL	m_strmLambda;
	BOOL	m_strmLambdaValid;
	BOOL	m_strmWN;
	BOOL	m_bVorticityVectors;
	int		m_lmsRegionSize;
	BOOL	m_lmsUse;
	BOOL	m_lmsVtkPositions;
	BOOL	m_lmsVtkDistances;
	int		m_vortexPixels;
	BOOL	m_RegionCrPoints;
	BOOL	m_bSeedStrm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgFindVortex)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDlgStreamlines* pDlgStrm;
	float* m_vtxPeaks;
	float LmsVelocityModulus(float* vel);
	float LmsPointDistance(float xa, float ya, float za, float xb, float yb, float zb);
	float m_lmsAverageV[3];
	void NormaliseFloat3D(float * in_out, int width, int height, int slices);
	void LmsComputeMatrixA(float** matrix_a, int x, int y, int z);
	void LmsSetVelocityGradients(int x, int y, int z);
	int* m_lmsPositions;
	void ComputeVelocityGradients();
	int* m_lambdaNegative;
	void NormaliseInt3D(int * in_out, int width, int height, int slices);
	float* m_lambdaLambda;
	CDlgVolView* pLambdaDlg;
	float* m_lambda11;
	float* m_lambda12;
	float* m_lambda13;
	float* m_lambda21;
	float* m_lambda22;
	float* m_lambda23;
	float* m_lambda31;
	float* m_lambda32;
	float* m_lambda33;
	float (*m_lambdaEigenValsReal)[3];
	void LMComputeEigenvalues2D(float aa, float ab, float ba, float bb, int x, int y);
	void LMComputeEigenvalues3D(float aa, float ab, float ac, float ba, float bb, float bc, 
										float ca, float cb, float cc, int x, int y, int s);
	int iMouseState;
	float* m_vorticityPlotX;
	float* m_vorticityPlotY;
	float* m_vorticityPlotZ;
	void ControlsDataReset();
	double WNGetPointRadius(int x, int y);
	int WNGetMinIdx(double* dist, int count);
	double WNWrapAngle(double angle);
	void WNGetCirclePoints(int radius, CArray<CPoint, CPoint> &points);
	int m_windingPlotHeight;
	int m_windingPlotWidth;
	float* m_windingPlot;
	BOOL bNoLoadInitialise;
	float* m_eigenReal;
	float* m_eigenImag;
	float (*m_eigenValsReal)[3];
	float (*m_eigenValsImag)[3];
	CDlgVolView* pVorticityDlg;
	CDlgVolView* pVelGradVolDlg;
	CDlgVolView* pWindingSelVolDlg;
	void ComputeEigenvalues3D(float aa, float ab, float ac, float ba, float bb, float bc, 
		float ca, float cb, float cc, int x, int y, int s);
	void ComputeEigenvalues2D(float aa, float ab, float ba, float bb, int x, int y);
	CDlgVolView* pVolDlg;
	float* m_thresVelMagn;
	float* m_velMagn;
	int FloatToInt(float number);
	float* m_velGrad_dudx;
	float* m_velGrad_dudy;
	float* m_velGrad_dudz;
	float* m_velGrad_dvdx;
	float* m_velGrad_dvdy;
	float* m_velGrad_dvdz;
	float* m_velGrad_dwdx;
	float* m_velGrad_dwdy;
	float* m_velGrad_dwdz;
	void WNComputeWindingNumber(/*CDataMRI* slice*/);

	// Generated message map functions
	//{{AFX_MSG(CDlgFindVortex)
	afx_msg void OnVortex3dload();
	afx_msg void OnVortexDataload();
	afx_msg void OnVortexVelmagn();
	afx_msg void OnVortexVelmagnView();
	virtual void OnCancel();
	afx_msg void OnVortexVelmagnThresGo();
	afx_msg void OnVortexVelmagnViewThres();
	afx_msg void OnChangeVelmagnEdit();
	virtual BOOL OnInitDialog();
	afx_msg void OnVortexGradmatGo();
	afx_msg void OnVortexGradmatView();
	afx_msg void OnGenerateVortex();
	afx_msg void OnVortexViewReal();
	afx_msg void OnVortexViewImag();
	afx_msg void OnVortexFlipU();
	afx_msg void OnVortexFlipV();
	afx_msg void OnVortexFlipW();
	afx_msg void OnVortexWnSelectSlice();
	afx_msg void OnVortexWindingGo();
	afx_msg void OnVortexWindingView();
	afx_msg void OnVortexViewTolerance();
	afx_msg void OnVortexVorticityGo();
	afx_msg void OnVortexVorticityView();
	afx_msg void OnVortexLambdaGo();
	afx_msg void OnVortexLambdaView();
	afx_msg void OnVortexLambdaViewNegative(BOOL showdialog);
	afx_msg void OnVortexStrmView();
	afx_msg void OnVortexVtkView();
	afx_msg void OnVortexVtkSelectedVectors();
	afx_msg void OnVortexStrmViewMask();
	afx_msg void OnDlgTracking();
	afx_msg void OnVortexVtkSaveArrows();
	afx_msg void OnViewLmsPositions();
	afx_msg void OnViewLmsDistances();
	afx_msg void OnVortexSavedata();
	afx_msg void OnMoreVtx();
	afx_msg void OnVortexSaveData3D();
	afx_msg void OnFindvortexDicom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFINDVORTEX_H__DF4CCE03_0C25_4DE3_AF0D_13FDF0EC5DE7__INCLUDED_)
