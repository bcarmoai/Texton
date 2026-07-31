#if !defined(AFX_DLGCONTOUROPTIONS_H__8AEE6297_312C_48D8_BA74_FD5DE882CDC9__INCLUDED_)
#define AFX_DLGCONTOUROPTIONS_H__8AEE6297_312C_48D8_BA74_FD5DE882CDC9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgContourOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgContourOptions dialog

class CDlgContourOptions : public CDialog
{
// Construction
public:
	CFrameWnd * pFrame;
	CDlgContourOptions(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgContourOptions)
	enum { IDD = IDD_CONTOUR_OPTS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgContourOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgContourOptions)
	afx_msg void OnDraw();
	afx_msg void OnErase();
	afx_msg void OnClear();
	afx_msg void OnClose();
	afx_msg void ContourOnImport();
	afx_msg void OnEllipse();
	afx_msg void OnCircle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONTOUROPTIONS_H__8AEE6297_312C_48D8_BA74_FD5DE882CDC9__INCLUDED_)
