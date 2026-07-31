#if !defined(AFX_CANNYDLG_H__0F5FDE93_A097_411C_99D0_25C675C32B03__INCLUDED_)
#define AFX_CANNYDLG_H__0F5FDE93_A097_411C_99D0_25C675C32B03__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CannyDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCannyDlg dialog

class CCannyDlg : public CDialog
{
// Construction
public:
	CView* pOwnerView;
	CCannyDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCannyDlg)
	enum { IDD = IDD_EDGEDET };
	CSpinButtonCtrl	m_upperSpin;
	CSpinButtonCtrl	m_stdSPin;
	CSpinButtonCtrl	m_lowSpin;
	float	m_std;
	int		m_lowThres;
	int		m_hiThres;
	BOOL	m_showOriginal;
	BOOL	m_magShade;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCannyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCannyDlg)
	afx_msg void OnApply();
	afx_msg void OnEdgeSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CANNYDLG_H__0F5FDE93_A097_411C_99D0_25C675C32B03__INCLUDED_)
