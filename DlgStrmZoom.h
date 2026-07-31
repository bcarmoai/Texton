#if !defined(AFX_DLGSTRMZOOM_H__D657BAFA_BAA2_420E_A822_10450E96C21A__INCLUDED_)
#define AFX_DLGSTRMZOOM_H__D657BAFA_BAA2_420E_A822_10450E96C21A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgStrmZoom.h : header file
//

#include "DlgStreamlines.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgStrmZoom dialog

class CDlgStrmZoom : public CDialog
{
// Construction
public:
	void TakeZoomText(CString text);
	CDlgStreamlines* pDlgStrm;
	CDlgStrmZoom(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgStrmZoom)
	enum { IDD = IDD_STRM_ZOOM };
	CString	m_ZoomCoords;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStrmZoom)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgStrmZoom)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSTRMZOOM_H__D657BAFA_BAA2_420E_A822_10450E96C21A__INCLUDED_)
