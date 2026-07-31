#if !defined(AFX_DLGTEXTBOX_H__D945B0B2_4AF8_46A2_B803_C67ADC68A7A6__INCLUDED_)
#define AFX_DLGTEXTBOX_H__D945B0B2_4AF8_46A2_B803_C67ADC68A7A6__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgTextBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTextBox dialog

class CDlgTextBox : public CDialog
{
// Construction
public:
	BOOL* isActive;
	void ClearAllText();
	void AddText(CString text);
	CDlgTextBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTextBox)
	enum { IDD = IDD_TEXTOUT };
	CEdit	m_editText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTextBox)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTextBox)
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTEXTBOX_H__D945B0B2_4AF8_46A2_B803_C67ADC68A7A6__INCLUDED_)
