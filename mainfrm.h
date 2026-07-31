// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__F2460098_79D6_4147_AA14_2029CF829F27__INCLUDED_)
#define AFX_MAINFRM_H__F2460098_79D6_4147_AA14_2029CF829F27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "DWnd.h"
#include "DlgTasks.h"
#include "DlgContourOptions.h"

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	void TasksVortex();
	void SetStatusBarText(CString text);
	void TasksRestoration();
	void TasksStreamlines();
	void TasksNoiseImg();
	void TasksIFTW();
	void TasksVoorhees();
	void TasksArcFourier();
	void TasksArcFit();
	void TasksHough();
	void TasksTemplate();
	void TasksOriginal();
	void ClosedDlgContour();
	void TasksContourDraw();
	void TasksThreshold();
	void TasksCanny();
	virtual ~CMainFrame();
	CDlgTasks * pDlgTasks;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CDlgContourOptions * pDlgContourOptions;


// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewTasks();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F2460098_79D6_4147_AA14_2029CF829F27__INCLUDED_)
