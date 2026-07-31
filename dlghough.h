#if !defined(AFX_DLGHOUGH_H__32EDC8CE_EE77_477C_BD1E_FA823C4C3317__INCLUDED_)
#define AFX_DLGHOUGH_H__32EDC8CE_EE77_477C_BD1E_FA823C4C3317__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgHough.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgHough dialog

class CDlgHough : public CDialog
{
// Construction
public:
	CView* pView;
	CDlgHough(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgHough)
	enum { IDD = IDD_HOUGH_OPTS };
	float	m_DiamMax;
	float	m_DiamMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgHough)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_iCircles;

	// Generated message map functions
	//{{AFX_MSG(CDlgHough)
	afx_msg void OnHoughApply();
	afx_msg void OnHoughCircles();
	afx_msg void OnHoughEllipses();
	virtual BOOL OnInitDialog();
	afx_msg void OnHoughSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHOUGH_H__32EDC8CE_EE77_477C_BD1E_FA823C4C3317__INCLUDED_)
