#if !defined(AFX_DLGARCFIT_H__AB78FE07_55D3_47C6_9B6D_7AFD94F19A42__INCLUDED_)
#define AFX_DLGARCFIT_H__AB78FE07_55D3_47C6_9B6D_7AFD94F19A42__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgArcFit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgArcFit dialog

class CDlgArcFit : public CDialog
{
// Construction
public:
	void CallbackText(char* text);
	CView* pView;
	CDlgArcFit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgArcFit)
	enum { IDD = IDD_ARCFIT };
	int		m_minPoints;
	float	m_maxError;
	CString	m_ProgressText;
	CString	m_ImgSizeTxt;
	int		m_maxPoints;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgArcFit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgArcFit)
	afx_msg void OnApply();
	afx_msg void OnButton1();
	afx_msg void OnArcfitRedraw();
	afx_msg void OnArcfitCanny();
	afx_msg void OnArcfitBins();
	afx_msg void OnArcfitOrigreen();
	afx_msg void OnArcfitOrytocanny();
	afx_msg void OnArcfitClaims();
	afx_msg void OnArcfitClaimarcs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGARCFIT_H__AB78FE07_55D3_47C6_9B6D_7AFD94F19A42__INCLUDED_)
