#if !defined(AFX_DLG7DLOADOPTIONS_H__146D1820_6422_44EB_990D_114B85E22274__INCLUDED_)
#define AFX_DLG7DLOADOPTIONS_H__146D1820_6422_44EB_990D_114B85E22274__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg7DLoadOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg7DLoadOptions dialog

class CDlg7DLoadOptions : public CDialog
{
// Construction
public:
	CDlg7DLoadOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg7DLoadOptions)
	enum { IDD = IDD_RESTOR_LOAD7D };
	int		m_timeFrame;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg7DLoadOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlg7DLoadOptions)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG7DLOADOPTIONS_H__146D1820_6422_44EB_990D_114B85E22274__INCLUDED_)
