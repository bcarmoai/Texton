#if !defined(AFX_DLGVORTEXVTK_H__8944D371_FCF6_4478_A6E3_CD5182791A85__INCLUDED_)
#define AFX_DLGVORTEXVTK_H__8944D371_FCF6_4478_A6E3_CD5182791A85__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgVortexVtk.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgVortexVtk dialog

class CDlgVortexVtk : public CDialog
{
// Construction
public:
	CDlgVortexVtk(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgVortexVtk)
	enum { IDD = IDD_VORTEX_VTK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgVortexVtk)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgVortexVtk)
	afx_msg void OnVortexVtkGo();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVORTEXVTK_H__8944D371_FCF6_4478_A6E3_CD5182791A85__INCLUDED_)
