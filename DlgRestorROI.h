#if !defined(AFX_DLGRESTORROI_H__45AD1A52_E8B0_49FF_9A2F_F5ECBEFBCA0D__INCLUDED_)
#define AFX_DLGRESTORROI_H__45AD1A52_E8B0_49FF_9A2F_F5ECBEFBCA0D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgRestorROI.h : header file
//

#include "DlgRestoration.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRestorROI dialog

class CDlgRestorROI : public CDialog
{
// Construction
public:
	CString GetText();
	void TakeRoiText(CString text);
	CDlgRestoration* pDlgRestor;
	CDlgRestorROI(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRestorROI)
	enum { IDD = IDD_REST_ROI };
	CString	m_CoordsText;
	BOOL	m_bUserTyped;
	int		m_edTopX;
	int		m_edTopY;
	int		m_edBottomX;
	int		m_edBottomY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRestorROI)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRestorROI)
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnRoiWhole();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESTORROI_H__45AD1A52_E8B0_49FF_9A2F_F5ECBEFBCA0D__INCLUDED_)
