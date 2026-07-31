#if !defined(AFX_DLGMOREVTX_H__CC5B2676_66EF_46AA_893C_413ADBEB5DAA__INCLUDED_)
#define AFX_DLGMOREVTX_H__CC5B2676_66EF_46AA_893C_413ADBEB5DAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgMoreVtx.h : header file
//

#include "FlowCluster.h"
#include "FlowAbstraction.h"
#include "Point3D.h"
#include "Data3DMRI.h"
#include "PointFloatArray.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgMoreVtx dialog

class CDlgMoreVtx : public CDialog
{
// Construction
public:
	~CDlgMoreVtx();
	void DetectCriticalPoints();
	int* m_clusterMap;
	int img_slices;
	int img_height;
	int img_width;
	CDialog* pFindVortex;
	CDlgMoreVtx(CWnd* pParent = NULL);   // standard constructor
	CPoint3D* m_CrPts;	
	int m_nCrPts;		
	FlowCluster	*m_pFlowCluster;
	int	m_nCluster;
	// CRect m_boundRect;
	// BSC 14/11/03
	
// Dialog Data
	//{{AFX_DATA(CDlgMoreVtx)
	enum { IDD = IDD_VTX_MORE };
	float	m_RegionCompression;
	int		m_RegionClusters;
	CString	m_loadedTxt;
	CString	m_sStrmCountTxt;
	float	m_xLengthThres;
	CString	m_scorrTxt;
	int		m_nCluSizeThres;
	CString	m_nCluStreamTxt;
	float	m_minCurlThres;
	int		m_magnStrmThres;
	CString	m_sInfoTxt;
	CString	m_sInfoRanges;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMoreVtx)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int FloatToInt(float fnum);
	void GetClustersVelocity(CData3DMRI& data3D);
	void GetClustersOriginalVelocity(CData3DMRI& data3D);
	CPoint3D ComputeVortexCentre(BOOL b3D, float** matrix_a, CPoint3D average_x, 
		CVector3D average_v);
	FlowAbstraction* m_pFlowAbstraction;
	float m_compressRatio;
	void UpdateClusters();
	int iClustersCompression;
	CVector3D* m_fitVec;
	void ComputeEigenvalues(BOOL dim3D, float** matrix_a, float& real, float& imag);
	CPointFloatArray* m_StrmPointsArray;
	CPointFloatArray* m_SelectedStrmPointsArray;
	CPointFloatArray* m_magCurlSelectedArray;
	int m_nStrmCount;
	int m_nSelectedStrmCount;
	int m_nMagCurlSelectedCount;

	// Generated message map functions
	//{{AFX_MSG(CDlgMoreVtx)
	afx_msg void OnRegionSelectCompression();
	afx_msg void OnRegionSelectClusters();
	afx_msg void OnRegionGo();
	afx_msg void OnRegionSaveArrows();
	afx_msg void OnRegionSavePoints();
	afx_msg void OnRegionViewClusters();
	virtual void OnCancel();
	afx_msg void OnViewBin();
	virtual BOOL OnInitDialog();
	afx_msg void OnButton1();
	afx_msg void OnRegionsSaveVelocity();
	afx_msg void OnRegionSaveClusters();
	afx_msg void OnRegionsLoadClusters();
	afx_msg void OnRegionsSeedDistances();
	afx_msg void OnSaveMatlabVelocity();
	afx_msg void OnSaveMatlabOriginal();
	afx_msg void OnStreamlinesGo();
	afx_msg void OnRegionStrmRenderAll();
	afx_msg void OnRegionsXCorrGo();
	afx_msg void OnRegionRenderXcorr();
	afx_msg void OnButton2();
	afx_msg void OnRegionRenderMinCurl();
	afx_msg void OnRegionRenderMinMag();
	afx_msg void OnRegionRenderMagnCurl();
	afx_msg void OnSaveMatlabXcorr();
	afx_msg void OnRegionSaveVtkXcorrVel();
	afx_msg float OnRegionsClusterInformation(float epsilon, float* argTerm1, 
											   float* argTerm2, float* argTerm3);
	afx_msg void OnRegionsInformationBatchGo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMOREVTX_H__CC5B2676_66EF_46AA_893C_413ADBEB5DAA__INCLUDED_)
