#if !defined(AFX_THRESDLG_H__E5FE4D0E_F6A8_48A5_B31C_5BB926C93491__INCLUDED_)
#define AFX_THRESDLG_H__E5FE4D0E_F6A8_48A5_B31C_5BB926C93491__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ThresDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThresDlg dialog

class CThresDlg : public CDialog
{
// Construction
public:
	CView* pOwnerView;
	CThresDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CThresDlg)
	enum { IDD = IDD_THRES };
	CSpinButtonCtrl	m_ctlSpinU;
	CSpinButtonCtrl	m_ctlSpinL;
	BOOL	m_bShowImage;
	int		m_iLower;
	int		m_iUpper;
	int		m_iRangeWidth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThresDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CThresDlg)
	afx_msg void OnApply();
	afx_msg void OnThresAnimate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THRESDLG_H__E5FE4D0E_F6A8_48A5_B31C_5BB926C93491__INCLUDED_)
