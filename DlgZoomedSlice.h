#if !defined(AFX_DLGZOOMEDSLICE_H__076BF76A_4B0C_4323_8420_482BA87340CF__INCLUDED_)
#define AFX_DLGZOOMEDSLICE_H__076BF76A_4B0C_4323_8420_482BA87340CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgZoomedSlice.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgZoomedSlice dialog

class CDlgZoomedSlice : public CDialog
{
// Construction
public:
	CString m_psImgSize;
	int m_dirSelect;
	CDlgZoomedSlice(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgZoomedSlice)
	enum { IDD = IDD_ZOOMED_RESLICE };
	CString	m_sImgSize;
	int		m_z;
	int		m_x;
	int		m_y;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgZoomedSlice)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgZoomedSlice)
	afx_msg void OnOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusZ();
	afx_msg void OnSetfocusX();
	afx_msg void OnSetfocusY();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGZOOMEDSLICE_H__076BF76A_4B0C_4323_8420_482BA87340CF__INCLUDED_)
