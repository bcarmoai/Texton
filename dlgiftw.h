#if !defined(AFX_DLGIFTW_H__CC28994C_0D3C_43F7_B71F_4FF43A071AA5__INCLUDED_)
#define AFX_DLGIFTW_H__CC28994C_0D3C_43F7_B71F_4FF43A071AA5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgIFTW.h : header file
//

#include "ImagePgm.h"
#include "DlgVolView.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgIFTW dialog

class CDlgIFTW : public CDialog
{
// Construction
public:
	void TakeDownPoint(CPoint point);
	BOOL bZoom;
	BOOL bInteract;
	int* imgCosts;
	CDlgIFTW(CWnd* pParent = NULL);   // standard constructor
	CView* pView;
	CImagePgm imgDots;
	void TakeMovePoint(CPoint point);


// Dialog Data
	//{{AFX_DATA(CDlgIFTW)
	enum { IDD = IDD_IFTW };
	CString	m_staticDots;
	CString	m_mouseRootTitle;
	CString	m_mouseRootValue;
	int		m_macCCost;
	int		m_cirRad;
	int		m_maxIterations;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgIFTW)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void PropagNewWatershed(CPoint oldRoot, CPoint newRoot);
	CPoint interactRootPoint;
	int* green_paths;
	CPoint* predec_map;
	CPoint* root_map;
	CDlgVolView* pVolDlg;
	int* imgIftwAnnotated;

	// Generated message map functions
	//{{AFX_MSG(CDlgIFTW)
	afx_msg void OnPeaks();
	afx_msg void OnIftwShowPeaks();
	afx_msg void OnIftwOriginal();
	afx_msg void OnIftwWatershed();
	afx_msg void OnIftwShowW();
	afx_msg void OnIftwCosts();
	afx_msg void OnIftwShowCosts();
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnIftwInteract();
	afx_msg void OnIftwPropag();
	afx_msg void OnIftwCheckRoots();
	afx_msg void OnIftwSavematlab();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIFTW_H__CC28994C_0D3C_43F7_B71F_4FF43A071AA5__INCLUDED_)
