#if !defined(AFX_DLGTASKS_H__548209EB_BB59_447C_BF51_1DBD0444E8A5__INCLUDED_)
#define AFX_DLGTASKS_H__548209EB_BB59_447C_BF51_1DBD0444E8A5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// DlgTasks.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTasks dialog

class CDlgTasks : public CDialog
{
// Construction
public:
	CDlgTasks(CWnd* pParent = NULL);   // standard constructor
	CFrameWnd* pFrame;
// Dialog Data
	//{{AFX_DATA(CDlgTasks)
	enum { IDD = IDD_TASKS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTasks)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTasks)
	afx_msg void OnCanny();
	afx_msg void OnContourDraw();
	afx_msg void OnThreshold();
	afx_msg void OnOriginal();
	afx_msg void OnConttmplGo();
	afx_msg void OnHoughGo();
	afx_msg void OnArcGo();
	afx_msg void OnFourierGo();
	afx_msg void OnVoorheesGo();
	afx_msg void OnIftwGo();
	afx_msg void OnNoiseimgGo();
	afx_msg void OnStreamlines();
	afx_msg void OnRestorationGo();
	afx_msg void OnVortexGo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTASKS_H__548209EB_BB59_447C_BF51_1DBD0444E8A5__INCLUDED_)
