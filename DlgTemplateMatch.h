#if !defined(AFX_DLGTEMPLATEMATCH_H__E56D0565_D7DE_4085_AE3C_18FB410155D3__INCLUDED_)
#define AFX_DLGTEMPLATEMATCH_H__E56D0565_D7DE_4085_AE3C_18FB410155D3__INCLUDED_

#include "TemplateMatcher.h"	// Added by ClassView
#include "DlgTextBox.h"
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgTemplateMatch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTemplateMatch dialog
//#include "TemplateMatcher.h"

class CDlgTemplateMatch : public CDialog
{
// Construction
public:
	void PrintProgress(int percent);
	CView * pView;
	CDlgTemplateMatch(CWnd* pParent = NULL);   // standard constructor
	CDlgTextBox* debugText; 
	BOOL isActive;

// Dialog Data
	//{{AFX_DATA(CDlgTemplateMatch)
	enum { IDD = IDD_TMPL };
	CButton	m_tmplErrorThresCtrl;
	CStatic	m_FinalParmsCtrl;
	CButton	m_DrawErrorCtrl;
	CButton	m_DrawFitCtrl;
	CProgressCtrl	m_ProgressBar;
	CStatic	m_ProgressCtrl;
	CString	m_ProgressText;
	float		m_initAngle;
	float		m_initCX;
	float		m_initCY;
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
	CString	m_FinalParms;
	float	m_weightINT;
	float	m_weigthWP;
	float	m_trainWP;
	float	m_trainLW;
	float	m_trainLH;
	float	m_weigthLW;
	float	m_weigthLH;
	BOOL	m_positionFixed;
	BOOL	m_bSaveCallParms;
	float	m_trainG;
	float	m_trainW;
	float	m_weigthMH;
	BOOL	m_templFixed;
	float	m_trainANG;
	float	m_errorThres;
	float	m_tmplClusters;
	float	m_tmplDOWNF;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTemplateMatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CoordsSmallToLarge(float* xpos, float* ypos, int scale, int classes);
	int* binErrorMap;
	void FindCandidateClusters(int* error_map, int width, int height, float** xpos, float** ypos, int* clusters, 
		int* total_clusters);
	void OnMatchDownsampled(int* image, int height, int width);
	CTemplateMatcher m_TemplateMatcher;

	// Generated message map functions
	//{{AFX_MSG(CDlgTemplateMatch)
	afx_msg void OnMatchStart(BOOL buttonPress = TRUE, float* final_parms = NULL);
	afx_msg void OnDrawError();
	afx_msg void OnTmplDrawfit();
	afx_msg void OnTmplDrawinit();
	afx_msg void OnTmplStart2(BOOL buttonPress = TRUE);
	afx_msg void OnButton1();
	afx_msg void OnTmplDSTC();
	afx_msg void OnTmplDrawori();
	afx_msg void OnDrawThres();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEMPLATEMATCH_H__E56D0565_D7DE_4085_AE3C_18FB410155D3__INCLUDED_)
