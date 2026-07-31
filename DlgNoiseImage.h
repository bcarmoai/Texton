#if !defined(AFX_DLGNOISEIMAGE_H__C891EA3A_3799_475A_B153_438D847E304A__INCLUDED_)
#define AFX_DLGNOISEIMAGE_H__C891EA3A_3799_475A_B153_438D847E304A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgNoiseImage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNoiseImage dialog

class CDlgNoiseImage : public CDialog
{
// Construction
public:
	CDlgNoiseImage(CWnd* pParent = NULL);   // standard constructor
	CView* pView;

// Dialog Data
	//{{AFX_DATA(CDlgNoiseImage)
	enum { IDD = IDD_NOISEIMG };
	UINT	m_Seed;
	int	m_Std;
	int	m_Mean;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNoiseImage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNoiseImage)
	afx_msg void OnNoiseGo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGNOISEIMAGE_H__C891EA3A_3799_475A_B153_438D847E304A__INCLUDED_)
